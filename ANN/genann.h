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


#ifndef GENANN_H
#define GENANN_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GENANN_RANDOM
/* 对于0和1之间的一致随机数，我们使用以下公式.
 * 如果你有一个更好的函数，重新定义这个宏。 */
#define GENANN_RANDOM() (((double)rand())/RAND_MAX)
#endif

struct genann;

typedef double (*genann_actfun)(const struct genann *ann, double a);

typedef struct genann {
    int inputs;         //输入层神经元数
    int hidden_layers;  //隐藏层层数
    int hidden;		    //隐藏层神经元数
    int outputs;		//输出层神经元数

    /* 用于隐藏神经元的激活函数。默认值：Gennann_act_sigmoid_cached */
    genann_actfun activation_hidden;

    /* 用于输出的激活功能。默认值：Gennann_act_sigmoid_cached */
    genann_actfun activation_output;

    /* 权重总数和权重缓冲区大小. */
    int total_weights;

    /* 神经元总数+输入和输出缓冲区大小. */
    int total_neurons;

    /* 所有权重（total_Weights Long）. */
    double *weight;

    /* 存储每个神经元的输入阵列和输出（total_neurons Long）. */
    double *output;

    /* 存储每个隐藏和输出神经元的增量(total_neurons - inputs long). */
    double *delta;

} genann;

/* 创造并返回新的 ann. */
genann *genann_init(int inputs, int hidden_layers, int hidden, int outputs);

/* 从用Genann_write保存的文件创建ANN. */
genann *genann_read(FILE *in);

/* 随机设置权重。由Init调用 */
void genann_randomize(genann *ann);

/* 返回Ann的新副本. */
genann *genann_copy(genann const *ann);

/* 释放 Ann. */
void genann_free(genann *ann);

/* 运行前馈算法来计算Ann的输出。 */
double const *genann_run(genann const *ann, double const *inputs);

/* 单个背包是否更新. */
void genann_train(genann const *ann, double const *inputs, double const *desired_outputs, double learning_rate);

/* 储存 ann. */
void genann_write(genann const *ann, FILE *out);

void genann_init_sigmoid_lookup(const genann *ann);
double genann_act_sigmoid(const genann *ann, double a);
double genann_act_sigmoid_cached(const genann *ann, double a);
double genann_act_threshold(const genann *ann, double a);
double genann_act_linear(const genann *ann, double a);


#ifdef __cplusplus
}
#endif

#endif /*GENANN_H*/
