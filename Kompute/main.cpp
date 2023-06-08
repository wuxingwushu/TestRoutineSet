#include <iostream>
#include "Kompute.hpp"

int main() {

    // 1. Create Kompute Manager with default settings (device 0 and first compute compatible queue)
    kp::Manager mgr;

    // 2. Create and initialise Kompute Tensors through manager
    auto tensorInA = mgr.buildTensor({ 2., 2., 2. });
    auto tensorInB = mgr.buildTensor({ 1., 2., 3. });
    auto tensorOut = mgr.buildTensor({ 0., 0., 0. });

    auto dd = tensorOut.get()->constructDescriptorBufferInfo().buffer;

    // 3. Specify "multiply shader" code (can also be raw string, spir-v bytes or file path)
    std::string shaderString = (R"(
        #version 450

        layout (local_size_x = 1) in;

        // The input tensors bind index is relative to index in parameter passed
        layout(set = 0, binding = 0) buffer bina { float tina[]; };
        layout(set = 0, binding = 1) buffer binb { float tinb[]; };
        layout(set = 0, binding = 2) buffer bout { float tout[]; };

        void main() {
            uint index = gl_GlobalInvocationID.x;
            tout[index] = tina[index] * tinb[index];
        }
    )");

    // 3. Run operation with string shader synchronously
    mgr.evalOpDefault<kp::OpAlgoBase>(
        { tensorInA, tensorInB, tensorOut },
        std::vector<char>(shaderString.begin(), shaderString.end()));

    // 4. Map results back from GPU memory to print the results
    mgr.evalOpDefault<kp::OpTensorSyncLocal>({ tensorInA, tensorInB, tensorOut });

    // Prints the output which is Output: { 2, 4, 6 }
    for (const float& elem : tensorOut->data()) std::cout << elem << "  ";
}

