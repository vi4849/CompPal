## Title
Intel&reg; oneAPI Rendering Toolkit Learning Path
  
## Requirements
| Optimized for                     | Description
|:---                               |:---
| OS                                | Linux* Ubuntu 18.04, 20 Windows* 10
| Hardware                          | Skylake newer
| Software                          | Intel&reg; oneAPI Rendering Toolkit, Jupyter Notebooks, Intel DevCloud

# Intel&reg; oneAPI Rendering Toolkit (Render Kit) Jupyter Notebook Training Content
This repo contains Jupyter Notebook Trainings for Render Kit that can be used on the Intel DevCloud for hands-on workshops and other training.

At the end of this course, you will be able to:

- Create high-fidelity photorealistic images using the Intel OSPRay renderer.
- Use the Embree API to execute ray-surface intersection tests required for performant ray-tracing applications.
- Use the Open VKL API to execute ray-volumetric hit queries required for performant rendering of volumetric objects.
- Use Intel Open Image Denoise to reducing the amount of necessary samples per pixel in ray tracing-based rendering applications by filtering out noise inherent to stochastic ray tracing methods.

## License  
Code samples are licensed under the MIT license. See [License.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/License.txt) for details.

Third party program Licenses can be found here: [third-party-programs.txt](https://github.com/oneapi-src/oneAPI-samples/blob/master/third-party-programs.txt)

## Content Details

#### Pre-requisites
- C++ Programming

#### Training Modules

| Modules | Description
|---|---|
|[Render Kit Introduction](01_RenderKit_Intro/RenderKit_Intro.ipynb)| + Introduction and Motivation for the Intel oneAPI Rendering Toolkit components.
|[Introduction to Intel OSPRAY](osp1_OSPRAY_Intro/OSPRAY_Intro.ipynb)| + An introduction to OSPRAY, a high-performance ray-tracing renderer for scientific visualization and high-fidelity photorealistic rendering.
|[OSPRAY Techniques and Procedural Scenes](osp2_ospExamples/ospExamples.ipynb)| + A compendium of basic OSPRay API techniques.
|[OSPRAY Asset Loader](osp3_asset_load/asset_loader.ipynb)| + A minimal OBJ file loader to import complex models for OSPRay to render.
|[Getting started with Intel Embree](embree1_minimal/embree_minimal.ipynb)| + Getting started with Embree. A demonstration of how to initialize a device and scene, and how to intersect rays with the scene. 
|[Embree Triangle Geometry](embree2_triangle_geometry/embree_triangle_geometry.ipynb)| + Demonstrates the creation of a static cube and ground plane using triangle meshes.
|[Open VKL Tutorial](vkl1_openvkl_tutorial/openvkl_tutorial.ipynb)| + This module creates a simple procedural regular structured volume and uses the various API version iterate using scalar, vector, and stream methods.
|[VDB Volume in Open VKL](vkl2_openvkl_vdbvkl/openvkl_vdbvkl.ipynb)| + This module demonstrates the use of a VDB volume structure using OpenVDB for defining volumetric data.
|[Intel® Open Image Denoise](oidn/OIDN.ipynb)| + Code walkthrough of the Intel Open Image Denoise library.

#### Content Structure

Each module folder has a Jupyter Notebook file (`*.ipynb`), this can be opened in Jupyter Lab to view the training contant, edit code and compile/run. Along with the Notebook file, there is a `lab` and a `src` folder with SYCL source code for samples used in the Notebook. The module folder also has `run_*.sh` files which can be used in shell terminal to compile and run each sample code.

## Install Directions

The training content can be accessed locally on the computer after installing necessary tools, or you can directly access using Intel DevCloud without any installation.

#### Access using Intel DevCloud

The Jupyter notebooks are tested and can be run on Intel DevCloud without any installation necessary, below are the steps to access these Jupyter notebooks on Intel DevCloud:
1. Register on [Intel DevCloud](https://devcloud.intel.com/oneapi)
2. Login, Get Started and Launch Jupyter Lab
3. Open Terminal in Jupyter Lab and git clone the repo and access the Notebooks

#### Local Installation of oneAPI Tools and JupyterLab

The Jupyter Notebooks can be downloaded locally to computer and accessed:
- Install Intel oneAPI Base Toolkit on local computer: [Installation Guide](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html) 
- Install Jupyter Lab on local computer: [Installation Guide](https://jupyterlab.readthedocs.io/en/stable/getting_started/installation.html)
- git clone the repo and access the Notebooks using Jupyter Lab

#### Local Installation of oneAPI Tools and use command line

The Jupyter Notebooks can be viewed on Github and you can run the code on command line:
- Install Intel oneAPI Base Toolkit on local computer (linux): [Installation Guide](https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html)
- git clone the repo
- open command line terminal and use the `run_*.sh` script in each module to compile and run code.