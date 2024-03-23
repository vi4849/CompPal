#path_to_symbolic=~/Training/Renderkit_w/link_to_IRTK_BD
path_to_symbolic=../../../link_to_IRTK_BD
echo $path_to_symbolic
export openvkl_DIR=$path_to_symbolic/OpenVKL_BD/openvkl-2.0.0.sycl.x86_64.linux
export LD_LIBRARY_PATH=$path_to_symbolic/OpenVKL_BD/openvkl-2.0.0.sycl.x86_64.linux/lib:${LD_LIBRARY_PATH}
rm -r build
mkdir build
cd build
cmake ..
cmake --build . --verbose