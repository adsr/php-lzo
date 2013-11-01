php-lzo
=======

lzo compression for php

To install:

    $ sudo apt-get install php5-dev liblzo2-dev # or equivalent
    $ git clone https://github.com/adsr/php-lzo.git
    $ cd php-lzo
    $ phpize && ./configure && make
    $ php -denable_dl=on -dextension=`pwd`/modules/lzo.so lzo.php
    $ sudo make install
