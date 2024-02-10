#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include <iostream>


void read_data_train_image(std::vector<double>& Train_Data);
void read_data_test_image(std::vector<double>& Test_Data);
void read_data_train_label(std::vector<double>& Train_Label);
void read_data_test_label(std::vector<double>& Test_Label);