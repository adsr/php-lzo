<?php

if (!extension_loaded('lzo')) {
    dl('lzo.' . PHP_SHLIB_SUFFIX);
}

if (!function_exists('lzo_compress')) {
    echo "lzo module not found\n";
    exit(1);
}

$data = '';
for ($i = 0; $i < 100; $i++) {
    $data .= uniqid('', true);
}

$format = "    %16s %8s %10s\n";
printf($format, "Algorithm", "Pass?", "Compression ratio");
foreach (get_defined_constants() as $name => $alg) {
    if (!preg_match('/^LZO/', $name)) {
        continue;
    }
    $compressed_data = lzo_compress($data, $alg);
    $orig_data = lzo_decompress($compressed_data, $alg);
    //$orig_data = 'x';
    printf($format, $name, ($data === $orig_data ? 'yes' : 'no'), round(strlen($compressed_data) / strlen($data), 2));
}
