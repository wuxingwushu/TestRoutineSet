/*
 * GENANN - Minimal C Artificial Neural Network
 *
 * Copyright (c) 2015-2018 Lewis Van Winkle
 *
 * http://CodePlea.com
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgement in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 */

#include "genann.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef genann_act
#define genann_act_hidden genann_act_hidden_indirect
#define genann_act_output genann_act_output_indirect
#else
#define genann_act_hidden genann_act
#define genann_act_output genann_act
#endif

#define LOOKUP_SIZE 4096

double genann_act_hidden_indirect(const struct genann *ann, double a) {
    return ann->activation_hidden(ann, a);
}

double genann_act_output_indirect(const struct genann *ann, double a) {
    return ann->activation_output(ann, a);
}

const double sigmoid_dom_min = -15.0;
const double sigmoid_dom_max = 15.0;
double interval;
double lookup[LOOKUP_SIZE];

#ifdef __GNUC__
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#define unused          __attribute__((unused))
#else
#define likely(x)       x
#define unlikely(x)     x
#define unused
#pragma warning(disable : 4996) /* For fscanf */
#endif


double genann_act_sigmoid(const genann *ann unused, double a) {
    if (a < -45.0) return 0;//防止溢出
    if (a > 45.0) return 1;//防止溢出
    return 1.0 / (1 + exp(-a));//sigmoid函数
}

void genann_init_sigmoid_lookup(const genann *ann) {
        const double f = (sigmoid_dom_max - sigmoid_dom_min) / LOOKUP_SIZE;//步长
        int i;

        interval = LOOKUP_SIZE / (sigmoid_dom_max - sigmoid_dom_min);//步长
        for (i = 0; i < LOOKUP_SIZE; ++i) {//计算sigmoid函数值
            lookup[i] = genann_act_sigmoid(ann, sigmoid_dom_min + f * i);//计算sigmoid函数值
        }
}

double genann_act_sigmoid_cached(const genann *ann unused, double a) {
    assert(!isnan(a));//检查a是否为nan

    if (a < sigmoid_dom_min) return lookup[0];//防止溢出
    if (a >= sigmoid_dom_max) return lookup[LOOKUP_SIZE - 1];//防止溢出

    size_t j = (size_t)((a-sigmoid_dom_min)*interval+0.5);//计算索引

    /* 因为浮点... */
    if (unlikely(j >= LOOKUP_SIZE)) return lookup[LOOKUP_SIZE - 1];//防止溢出

    return lookup[j];//返回sigmoid函数值
}

double genann_act_linear(const struct genann *ann unused, double a) {
    return a;
}

double genann_act_threshold(const struct genann *ann unused, double a) {
    return a > 0;
}

genann *genann_init(int inputs, int hidden_layers, int hidden, int outputs) {
    if (hidden_layers < 0) return 0;//隐藏层数小于0
    if (inputs < 1) return 0;//输入层数小于1
    if (outputs < 1) return 0;//输出层数小于1
    if (hidden_layers > 0 && hidden < 1) return 0;//隐藏层数大于0且隐藏层数小于1


    const int hidden_weights = hidden_layers ? (inputs+1) * hidden + (hidden_layers-1) * (hidden+1) * hidden : 0;//隐藏层权重
    const int output_weights = (hidden_layers ? (hidden+1) : (inputs+1)) * outputs;//输出层权重
    const int total_weights = (hidden_weights + output_weights);//总权重

    const int total_neurons = (inputs + hidden * hidden_layers + outputs);//总神经元

    /* 为重量、输出和增量分配额外尺寸. */
    const int size = sizeof(genann) + sizeof(double) * (total_weights + total_neurons + (total_neurons - inputs));//分配内存
    genann *ret = malloc(size);//分配内存
    if (!ret) return 0;//分配内存失败

    ret->inputs = inputs;
    ret->hidden_layers = hidden_layers;
    ret->hidden = hidden;
    ret->outputs = outputs;

    ret->total_weights = total_weights;
    ret->total_neurons = total_neurons;

    /* 设置指针. */
    ret->weight = (double*)((char*)ret + sizeof(genann));//权重
    ret->output = ret->weight + ret->total_weights;//输出
    ret->delta = ret->output + ret->total_neurons; 

    genann_randomize(ret);

    ret->activation_hidden = genann_act_sigmoid_cached;
    ret->activation_output = genann_act_sigmoid_cached;

    genann_init_sigmoid_lookup(ret);//初始化sigmoid函数查找表

    return ret;
}


genann *genann_read(FILE *in) {
    int inputs, hidden_layers, hidden, outputs;
    int rc;

    errno = 0;
    rc = fscanf(in, "%d %d %d %d", &inputs, &hidden_layers, &hidden, &outputs);
    if (rc < 4 || errno != 0) {
        perror("fscanf");
        return NULL;
    }

    genann *ann = genann_init(inputs, hidden_layers, hidden, outputs);

    int i;
    for (i = 0; i < ann->total_weights; ++i) {
        errno = 0;
        rc = fscanf(in, " %le", ann->weight + i);
        if (rc < 1 || errno != 0) {
            perror("fscanf");
            genann_free(ann);

            return NULL;
        }
    }

    return ann;
}


genann *genann_copy(genann const *ann) {
    const int size = sizeof(genann) + sizeof(double) * (ann->total_weights + ann->total_neurons + (ann->total_neurons - ann->inputs));
    genann *ret = malloc(size);
    if (!ret) return 0;

    memcpy(ret, ann, size);

    /* Set pointers. */
    ret->weight = (double*)((char*)ret + sizeof(genann));
    ret->output = ret->weight + ret->total_weights;
    ret->delta = ret->output + ret->total_neurons;

    return ret;
}


void genann_randomize(genann *ann) {
    int i;
    for (i = 0; i < ann->total_weights; ++i) {
        double r = GENANN_RANDOM();
        /* 设置从-0.5到0.5的权重. */
        ann->weight[i] = r - 0.5;
    }
}


void genann_free(genann *ann) {
    /* 权重、输出和增量指针指向同一缓冲区. */
    free(ann);
}


double const *genann_run(genann const *ann, double const *inputs) {
    double const *w = ann->weight;//权重
    double *o = ann->output + ann->inputs;//输出
    double const *i = ann->output;//输入

    /* 将输入复制到划痕区，在那里我们还存储每个神经元的
     * 输出，用于一致性。这样第一层不是特例. */
    memcpy(ann->output, inputs, sizeof(double) * ann->inputs);

    int h, j, k;

    //没有隐藏层
    if (!ann->hidden_layers) {
        double *ret = o;
        for (j = 0; j < ann->outputs; ++j) {
            double sum = *w++ * -1.0;
            for (k = 0; k < ann->inputs; ++k) {
                sum += *w++ * i[k];
            }
            *o++ = genann_act_output(ann, sum);//输出层
        }

        return ret;
    }

    /* 图输入层 */
    for (j = 0; j < ann->hidden; ++j) {
        double sum = *w++ * -1.0;
        for (k = 0; k < ann->inputs; ++k) {
            sum += *w++ * i[k];
        }
        *o++ = genann_act_hidden(ann, sum);//隐藏层
    }

    i += ann->inputs;//输入

    /* 图隐藏层（如果有）. */
    for (h = 1; h < ann->hidden_layers; ++h) {
        for (j = 0; j < ann->hidden; ++j) {
            double sum = *w++ * -1.0;// w[0] is the bias.
            for (k = 0; k < ann->hidden; ++k) {
                sum += *w++ * i[k];
            }
            *o++ = genann_act_hidden(ann, sum);//隐藏层
        }

        i += ann->hidden;//输入
    }

    double const *ret = o;//输出

    /* 图输出层. */
    for (j = 0; j < ann->outputs; ++j) {
        double sum = *w++ * -1.0;
        for (k = 0; k < ann->hidden; ++k) {
            sum += *w++ * i[k];
        }
        *o++ = genann_act_output(ann, sum);//输出层
    }

    /* 卫生检查我们使用了所有权重并编写了所有输出. */
    assert(w - ann->weight == ann->total_weights);//权重
    assert(o - ann->output == ann->total_neurons);//输出神经元

    return ret;
}


void genann_train(genann const *ann, double const *inputs, double const *desired_outputs, double learning_rate) {
    /* 首先，我们必须向前运行网络. */
    genann_run(ann, inputs);

    int h, j, k;

    /* 首先设置输出层 Deltas. */
    {
        double const *o = ann->output + ann->inputs + ann->hidden * ann->hidden_layers; /* 第一个输出. */
        double *d = ann->delta + ann->hidden * ann->hidden_layers; /* First delta. */
        double const *t = desired_outputs; /* 第一个期望输出. */


        /* 设置输出层 Deltas. */
        if (genann_act_output == genann_act_linear ||
                ann->activation_output == genann_act_linear) {
            for (j = 0; j < ann->outputs; ++j) {
                *d++ = *t++ - *o++;
            }
        } else {
            for (j = 0; j < ann->outputs; ++j) {
                *d++ = (*t - *o) * *o * (1.0 - *o);
                ++o; ++t;
            }
        }
    }


    /* 设置隐藏层Deltas，从最后一层开始并向后工作. */
    /* 注意，在Hidden_layers==0的情况下跳过循环. */
    for (h = ann->hidden_layers - 1; h >= 0; --h) {

        /* 在该层中查找第一个输出和增量. */
        double const *o = ann->output + ann->inputs + (h * ann->hidden);
        double *d = ann->delta + (h * ann->hidden);

        /* 在下一层中查找第一个增量（可能被隐藏或输出）. */
        double const * const dd = ann->delta + ((h+1) * ann->hidden);

        /* 在下一层中查找第一个权重（可能被隐藏或输出）. */
        double const * const ww = ann->weight + ((ann->inputs+1) * ann->hidden) + ((ann->hidden+1) * ann->hidden * (h));

        for (j = 0; j < ann->hidden; ++j) {

            double delta = 0;

            for (k = 0; k < (h == ann->hidden_layers-1 ? ann->outputs : ann->hidden); ++k) {
                const double forward_delta = dd[k];
                const int windex = k * (ann->hidden + 1) + (j + 1);
                const double forward_weight = ww[windex];
                delta += forward_delta * forward_weight;
            }

            *d = *o * (1.0-*o) * delta;
            ++d; ++o;
        }
    }


    /* 训练输出. */
    {
        /* 查找第一个输出增量. */
        double const *d = ann->delta + ann->hidden * ann->hidden_layers; /* First output delta. */

        /* 查找第一个权重到第一个输出增量. */
        double *w = ann->weight + (ann->hidden_layers
                ? ((ann->inputs+1) * ann->hidden + (ann->hidden+1) * ann->hidden * (ann->hidden_layers-1))
                : (0));

        /* 在上一层中查找第一个输出. */
        double const * const i = ann->output + (ann->hidden_layers
                ? (ann->inputs + (ann->hidden) * (ann->hidden_layers-1))
                : 0);

        /* 设置输出层权重. */
        for (j = 0; j < ann->outputs; ++j) {
            *w++ += *d * learning_rate * -1.0;
            for (k = 1; k < (ann->hidden_layers ? ann->hidden : ann->inputs) + 1; ++k) {
                *w++ += *d * learning_rate * i[k-1];
            }

            ++d;
        }

        assert(w - ann->weight == ann->total_weights);
    }


    /* 训练隐藏层. */
    for (h = ann->hidden_layers - 1; h >= 0; --h) {

        /* 在此层中查找第一个增量. */
        double const *d = ann->delta + (h * ann->hidden);

        /* 查找此层的第一个输入. */
        double const *i = ann->output + (h
                ? (ann->inputs + ann->hidden * (h-1))
                : 0);

        /* 查找此层的第一个权重. */
        double *w = ann->weight + (h
                ? ((ann->inputs+1) * ann->hidden + (ann->hidden+1) * (ann->hidden) * (h-1))
                : 0);


        for (j = 0; j < ann->hidden; ++j) {
            *w++ += *d * learning_rate * -1.0;
            for (k = 1; k < (h == 0 ? ann->inputs : ann->hidden) + 1; ++k) {
                *w++ += *d * learning_rate * i[k-1];
            }
            ++d;
        }

    }

}


void genann_write(genann const *ann, FILE *out) {
    fprintf(out, "%d %d %d %d", ann->inputs, ann->hidden_layers, ann->hidden, ann->outputs);

    int i;
    for (i = 0; i < ann->total_weights; ++i) {
        fprintf(out, " %.20e", ann->weight[i]);
    }
}


