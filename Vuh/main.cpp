#include <iostream>
#include <fstream>
#include <vuh/vuh.h>

/// write array of rgba values to a ppm image.
auto write_ppm(const char* filename
	, const uint32_t* data, uint32_t width, uint32_t height
) -> void
{
	auto fout = std::ofstream(filename, std::ios::binary);
	fout << "P6" << "\n" << width << " " << height << " 255" << "\n";
	for (auto i = 0u; i < width * height; ++i) {
		fout.put(char(*data++));
		fout.put(char(*data++));
		fout.put(char(*data++));
		++data; // skip alpha channel
	}
}

void mandelbrot() {
	const auto width = 600;
	const auto height = 400;

	auto instance = vuh::Instance();
	auto device = instance.devices().at(0); // just get the first compute-capable device

	auto mandel = vuh::Array<uint32_t, vuh::mem::Host>(device, 4 * width * height);

	using Specs = vuh::typelist<uint32_t, uint32_t>;   // width, height of a workgroup
	struct Params { uint32_t width; uint32_t height; };   // width, height of an image
	auto program = vuh::Program<Specs, Params>(device, "mandelbrot.spv");
	program.grid(vuh::div_up(width, 32), vuh::div_up(height, 32))
		.spec(32, 32)({ width, height }, mandel);   // run the kernel

	write_ppm("mandelebrot.ppm", mandel.data(), width, height);
}


void saxpy(){
	unsigned int Size = 128;
	auto y = std::vector<float>(Size, 1.0f);
	auto x = std::vector<float>(Size, 20.0f);

	for (size_t i = 0; i < y.size(); i++)
	{
		std::cout << y[i] << " ";
	}
	std::cout << std::endl;

	auto instance = vuh::Instance();
	auto device = instance.devices().at(0);    // just get the first available device

	auto d_y = vuh::Array<float>(device, y);   // create device arrays and copy data
	auto d_x = vuh::Array<float>(device, x);

	using Specs = vuh::typelist<uint32_t>;     // shader specialization constants interface
	struct Params { uint32_t size; float a; };    // shader push-constants interface
	auto program = vuh::Program<Specs, Params>(device, "saxpy.spv"); // load shader
	program.grid(Size / 64).spec(64)({ Size, 0.1 }, d_y, d_x); // run once, wait for completion

	d_y.toHost(begin(y));                      // copy data back to host

	for (size_t i = 0; i < y.size(); i++)
	{
		std::cout << y[i] << " ";
	}
	std::cout << std::endl;
}

int main() {
	//saxpy();
	mandelbrot();

	auto instance = vuh::Instance();
	auto device = instance.devices().at(0);    // just get the first available device
	auto mandel = vuh::Array<int, vuh::mem::Host>(device, 500);
	mandel.buffer();
	return 0;
}