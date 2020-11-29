<?php
	try {
		$pdo = new PDO('mysql:host=localhost;dbname=estacion_01', 'root', '');
		echo "Conexión exitoso a la BD";
	}catch(PDOException $e){
		echo "<h1> Error conectando a BD</h1>";
		print "Error, ".$e->getMessage();
		die();
	}
?>