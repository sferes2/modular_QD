# MODULAR_QD

A sferes2 module for modular Quality-Diversity algorithm.

## Authors:
- Antoine Cully a.cully@imperial.ac.uk

## Other Contributors:
This new framework is based on the existing MAP-Elites module for sferes2 (https://github.com/sferes2/map_elites).
Therefore, many thanks to its authors:
- Jean-Baptiste Mouret jean-baptiste.mouret@inria.fr
- Danesh Tarapore


## Reference:
- Cully, A. & Demiris, Y. (2016). Quality and Diversity Optimization: A Unifying Modular Framework. IEEE Transactions on Evolutionary Computation (under review).

## Installation:
First you need to install Sferes2 (https://github.com/sferes2/sferes2) as Modular_QD is module for this framework.
```
cd /your/workspace/folder/
git clone https://github.com/sferes2/sferes2.git
```

At the same location, you can clone the Modular_qd repository:
```
git clone https://github.com/sferes2/modular_QD.git
```

In order to link Sferes with the Modular_QD, we need to create a symbolic link:
```
cd sferes2/exp/
ln -s ../../../modular_QD ./
```

The configuration of Sferes can then include the additional requirements for Modular_QD:
```
cd -   #back to sferes root folder
./waf configure --cpp14=yes --exp modular_QD
```

Experimental scenarios using the physical simulator Robdyn will need additional configurations, for instance:
```
./waf configure --cpp14=yes --exp modular_QD --simu /location/of/simulation --robdyn /location/of/robdyn --controller /location/of/the/hexapod/controller
```
for more detail, please refer to these repositories:
https://github.com/resibots/robdyn
https://github.com/resibots/hexapod_simu
https://github.com/resibots/hexapod_common

After the configuration, the experiments can directly be compiled with:
```
./waf --exp modular_QD
```
This command will compile all the scenrios and variants defined in `exp/modular_QD/wscript`. This may represent a long compilation time.
To specify which scenario needs to be compile, you can use this command:
```
./waf --exp modular_QD --target name_of_my_scenario_and_variant
```

For instance, if you want to quickly test the library, you can do:
```
./waf --exp modular_QD --target test_qd
```

As another example, if you want to compile the "scenario_arm" with the "Grid" container and the "Random" selection operator (this configuration corresponds to the MAP_Elites algorithms), you can use this command:
```
./waf --exp modular_QD --target scenario_arm_grid_random
```

The binary are located in the "build" folder. For instance, in order to run the previously compiled scenario:
```
./build/exp/modular_QD/scenario_arm_grid_random
```

## Creating your own scenario:
You can add you own scenario in the 'exp/modular_QD/wscript' script and call compile them with the commands introduced in the previous section.

