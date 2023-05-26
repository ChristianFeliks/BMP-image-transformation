# BMP-image-transformation
Simple BMP transformation tool.  
Created by Christian Feliks.  
This is a copy from my private repository and is meant for job application purposses.  

### USAGE:
$./bmp [FILE]... [OPTION]
  
### EXAMPLE:
  $ bmp -i input.bmp -o output.bmp -r
  
### OPTIONS:
  -r            rotate image right  
  -l            rotate image left  
  -h            flip image horizontally  
  -v            flip image vertically  
  -c y x h w    crop image from position [y,x] of giwen height and widht  
  -s factor     scale image by factor  
  -e string     extract colors  
  -o file       write output to file   
  -i file       read input from the file    
