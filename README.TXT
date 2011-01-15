This file contains some information about configuration, usage, plugin 
development guide etc.

1. About conf.ini
   The format for the content of conf.ini is each lib name takes up one line.
   e.g.
   # Configuration for CL-PIM.
   # TODOManager, manage your daily TODO information.
   libtodomanager.so
   # ....
   # END OF conf.ini

   Note, we can use '#'(quote for calification) to put comment following it.
   And other symbols is not allowed in conf.ini. That means you can put '#'
   before some .so file name, and the commented plugin will be removed from
   CL-PIM.


2. Usage
   2.1 Plugin Installation

   2.2 Plugin Remove
       Just call plugin management function, and remove the plugin which you
       do not need any more. NOTE, you have to restart CL-PIM to make the 
       modification take effect.

3. Compile
   3.1 CLPIM requires the following third party libs, including:
       libxerces-c 3.1, libconv 1.13.1, libnurse