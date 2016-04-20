This directory contains two kinds of configuration files. All files store [armadillo matrices](http://arma.sourceforge.net/docs.html#save_load_mat) with the `raw_ascii` file type.

  * `.config` files store information about the robot. They have to be created / adjusted by hand. Each column contains the values of one platform (first row equals lowest platform), and each column represents a device.
  * `.calibration` files can be created by scripts in `applications/calibration`. They apply only to the Pi they were created on. Each row represents a device, and the values in that row the adjustment values.
