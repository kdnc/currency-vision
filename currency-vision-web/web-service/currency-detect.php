<?php
	//------- Temporary code to check the call from Android is received to server ----------
	$file = 'output.txt';
	// Open the file to get existing content
	$current = file_get_contents($file);
	// Append a new person to the file
	//$current .= "John Smith\n";
	$current .= rand() . " - test\n";
	// Write the contents back to the file
	file_put_contents($file, $current);
	
	// Save the received image file in server
	// $base=$_REQUEST['image'];
    // $binary=base64_decode($base);
    // header('Content-Type: bitmap; charset=utf-8');
    // $file = fopen('uploaded_image.jpg', 'wb');
    // fwrite($file, $binary);
    // fclose($file);


	$filename = '50_note_on_black_background_1_(Medium).jpg';
	//file_put_contents("$filename", file_get_contents("php://input"));

	$cmd  = 'currency-vision-logic8 --input="'.$filename.'" --outdir="'.dirname($filename).'"';
	$last = exec($cmd, $out);
	echo 'last - ' . $last;
	echo '[out ]- ' . $out;
	if (strpos($last, 'Error:') === false)
	{
		$res = array('success' => true);

		foreach ($out as $i => $line)
		{
			if (preg_match('/\d+,\s*([^\s]+)\s+\((\d+)x(\d+)\)/i', $line, $m))
				$res['images'][] = array('src' => $m[1], 'width' => $m[2], 'height' => $m[3]);
		}
	}
	else
		$res = array('success' => false, 'msg' => $last);

	header('Content-Type: application/json');
	echo json_encode($res);
