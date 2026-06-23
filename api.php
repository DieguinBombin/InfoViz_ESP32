<?php

if (!file_exists("valor.txt")) {
    echo "0";
    exit;
}

echo trim(file_get_contents("valor.txt"));
