<?php
$file = 'people.txt';
// Open the file to get existing content
$current = file_get_contents($file);
// Append a new person to the file
//$current .= "John Smith\n";
$current .= rand() . " - test\n";
// Write the contents back to the file
file_put_contents($file, $current);

    $base=$_REQUEST['image'];
     $binary=base64_decode($base);
    header('Content-Type: bitmap; charset=utf-8');
    $file = fopen('uploaded_image.jpg', 'wb');
    fwrite($file, $binary);
    fclose($file);
    echo 'Image upload complete!!, Please check your php file directory……';
?>