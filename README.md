Image Processing App 

This application provides a suite of image processing methods to manipulate images. 
Below is a brief overview of the available functionalities. 

![App](https://github.com/user-attachments/assets/adde0cfe-f4bf-4841-93a2-4e9b2a8f7731)

Features 

• Invert Image: Reverses the pixel intensities of the image (e.g., black to white, white 
to black). 

• Equalization of Histogram: Enhances image contrast by redistributing pixel 
intensities based on the histogram. 

• Full Scale Histogram Stretch: Stretches the histogram of the image to the full 
intensity range to improve contrast. 

• Convolution: Applies a kernel to the image for effects like blurring, sharpening, or 
edge detection. Hard-coded convolution mask 5x5.

• Linear Heat Equation: 
    o Explicit: Solves the heat equation using an explicit finite difference scheme 
      (controlled by Time Step Count and Time Step Size, Sigma <= 0.25). 
    o Implicit: Solves the heat equation using an implicit scheme for better 
      stability. (controlled by Time Step Count and Time Step Size, Sigma > 0.25). 

• Perona-Malikova (Semi-Implicit): Applies anisotropic diffusion to reduce noise 
while preserving edges (controlled by K parameter, Time Step Count and Time Step 
Size, Sigma for data filtration by Heat equation). 

• Edge Detector: Identifies edges in the image using gradient-based techniques. 
(controlled by K parameter) 

• MFC (Mean Field Consistency): A method for image segmentation or denoising 
based on mean field theory. (controlled by Time Step Count and Time Step Size, 
Sigma for data filtration by Heat equation). 

• GMFC (Generalized Mean Field Consistency): An enhanced version of MFC with 
broader applicability. (controlled by K parameter, Time Step Count and Time Step 
Size, Sigma for data filtration by Heat equation). 

• Eikonal R-T Distance Function: Computes distance maps using the Eikonal 
equation with R-T iterations (controlled by R-T MaxIter). 

Usage 
1. Open the app and load your image. 
2. Use the Controls panel to adjust parameters such as Time Step Count, Time Step 
Size, Sigma, K, and R-T MaxIter. 
3. Select the desired image processing method from the menu. 
4. Apply the method and view the result in the image viewer. 

Notes 
• Adjust parameters carefully to achieve the desired effect, especially for iterative 
methods like the Linear Heat Equation and Eikonal R-T Distance Function. 
• Best value for K parameter is 0.01, as the app work with image data with values from 
<0,255>.  
