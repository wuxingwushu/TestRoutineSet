#include "TooL.h"



#define TEST_LIUKE0 10
#define TEST_LIUKE1 1

const char* train_images_idx3_ubyte_file = "./data/train-images.idx3-ubyte";
const char* train_labels_idx1_ubyte_file = "./data/train-labels.idx1-ubyte";
const char* test_images_idx3_ubyte_file = "./data/t10k-images.idx3-ubyte";
const char* test_labels_idx1_ubyte_file = "./data/t10k-labels.idx1-ubyte";

void read_data_train_image(std::vector<double>& Train_Data)
{
    char32_t MAGIC_NUMBER = 0;
    char32_t NUM_OF_IMAGES = 0;
    char32_t NUM_OF_ROWS = 0;
    char32_t NUM_OF_COLS = 0;
    FILE* fp = fopen(train_images_idx3_ubyte_file, "rb");
    if (fp == NULL)
    {
        printf("unable open MNIST_data/train-images.idx3-ubyte");
    }
    else
    {
        unsigned char NUM_OF_IMAGES_TMP;
        fread(&MAGIC_NUMBER, sizeof(char32_t), 1, fp);


        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES = NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP;


        fread(&NUM_OF_ROWS, sizeof(char32_t), 1, fp);
        fread(&NUM_OF_COLS, sizeof(char32_t), 1, fp);

        unsigned char tmp;
        for (int i = 0; i < NUM_OF_IMAGES * 28 * 28 / TEST_LIUKE1; i++)
        {
            fread(&tmp, sizeof(char), 1, fp);
            Train_Data.push_back((double)tmp / 256);
        }
        fclose(fp);
    }
}

void read_data_test_image(std::vector<double>& Test_Data)
{
    char32_t MAGIC_NUMBER = 0;
    char32_t NUM_OF_IMAGES = 0;
    char32_t NUM_OF_ROWS = 0;
    char32_t NUM_OF_COLS = 0;
    FILE* fp = fopen(test_images_idx3_ubyte_file, "rb");
    if (fp == NULL)
    {
        printf("unable open MNIST_data/train-images.idx3-ubyte");
    }
    else
    {
        unsigned char NUM_OF_IMAGES_TMP;
        fread(&MAGIC_NUMBER, 1, sizeof(char32_t), fp);

        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES = NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP;


        fread(&NUM_OF_ROWS, sizeof(char32_t), 1, fp);
        fread(&NUM_OF_COLS, sizeof(char32_t), 1, fp);

        unsigned char tmp;
        for (int i = 0; i < NUM_OF_IMAGES * 28 * 28 / TEST_LIUKE0; i++)
        {
            fread(&tmp, sizeof(char), 1, fp);
            Test_Data.push_back((double)tmp / 256);
        }
        fclose(fp);
    }
}

void read_data_train_label(std::vector<double>& Train_Label)
{
    char32_t MAGIC_NUMBER = 0;
    char32_t NUM_OF_IMAGES = 0;
    FILE* fp = fopen(train_labels_idx1_ubyte_file, "rb");
    if (fp == NULL)
    {
        printf("unable open MNIST_data/train-images.idx3-ubyte");
    }
    else
    {
        unsigned char NUM_OF_IMAGES_TMP;
        fread(&MAGIC_NUMBER, 1, sizeof(char32_t), fp);

        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES = NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP;

        unsigned char tmp;
        for (int i = 0; i < NUM_OF_IMAGES; i++)
        {
            fread(&tmp, sizeof(char), 1, fp);
            for (int i = 0; i < 10; i++)
                if (tmp == i)
                    Train_Label.push_back(1);
                else
                    Train_Label.push_back(0);
        }
        fclose(fp);
    }
}

void read_data_test_label(std::vector<double>& Test_Label)
{
    char32_t MAGIC_NUMBER = 0;
    char32_t NUM_OF_IMAGES = 0;
    FILE* fp = fopen(test_labels_idx1_ubyte_file, "rb");
    if (fp == NULL)
    {
        printf("unable open MNIST_data/train-images.idx3-ubyte");
    }
    else
    {
        unsigned char NUM_OF_IMAGES_TMP;
        fread(&MAGIC_NUMBER, 1, sizeof(char32_t), fp);
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES = NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP * 256;
        fread(&NUM_OF_IMAGES_TMP, sizeof(char), 1, fp);
        NUM_OF_IMAGES += NUM_OF_IMAGES_TMP;

        unsigned char tmp;
        for (int i = 0; i < NUM_OF_IMAGES; i++)
        {
            fread(&tmp, sizeof(char), 1, fp);

            for (int i = 0; i < 10; i++)
            {
                if (i == tmp)
                    Test_Label.push_back(1);
                else
                    Test_Label.push_back(0);
            }
        }
        fclose(fp);
    }
}