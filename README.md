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

To use:

    $big_data = 'holy mackerel mackerel mackerel';
    $small_data = lzo_compress($big_data);

or:

    $orig_data = lzo_decompress($previously_compressed_data);

The default compression algorithm is LZO1X-1, but you can also specify one:

    $data = lzo_compress($data, LZO1X_999);
    $data = lzo_decompress($data, LZO1X_999);

See available compression algorithms in `db.h`.

Visit http://www.oberhumer.com/opensource/lzo/ to learn more about lzo compression.
