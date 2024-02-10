#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "genann.h"
#include "TooL.h"


int main(int argc, char* argv[])
{
    std::vector<double> TrainData;
    std::vector<double> TrainLabel;
    read_data_train_image(TrainData);
    read_data_train_label(TrainLabel);

    genann* ann = nullptr;
    if (0) {
        /*这将使神经网络每次运行的初始化方式不同*/
        /*如果你没有得到一个好的结果，请再试一次以获得不同的结果*/
        srand(time(0));
        ann = genann_init(784, 3, 784, 10);
    }
    else {
        FILE* fpa = fopen("./data/mods.ann", "r");
        ann = genann_read(fpa);
        fclose(fpa);
    }

    if (1) {
        for (int i = 0; i < 60000; ++i) {
            printf("%d\n", i);
            genann_train(ann, &TrainData[i * 784], &TrainLabel[i * 10], 0.1);
        }
    }
    

    FILE* fpw = fopen("mods.ann", "w");
    genann_write(ann, fpw);
    fclose(fpw);


    std::vector<double> testData;
    std::vector<double> testLabel;
    read_data_test_image(testData);
    read_data_test_label(testLabel);

    int Erorr = 0;
    int Correct = 0;
    for (size_t i = 0; i < 1000; i++)
    {
        const double* output = genann_run(ann, &testData[i * 784]);
        int da = 0;
        for (size_t i = 0; i < 9; i++)
        {
            if (output[da] < output[i + 1]) {
                da = i + 1;
            }
        }
        if (testLabel[i * 10 + da] == 1) {
            ++Correct;
        }
        else {
            ++Erorr;
        }
    }
    printf("Correct: %d\n", Correct);
    printf("Erorr: %d\n", Erorr);
    printf("准确率: %f\n", double(Correct) / 1000.0f);
    
    genann_free(ann);
    return 0;
}
