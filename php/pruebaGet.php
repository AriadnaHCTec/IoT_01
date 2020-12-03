<?php
if ($_GET){
	$numero = $_GET["numero"];
	$apellido = $_GET["apellido"];
	$nombre = $_GET["nombre"];
	echo "datos recibidos por get";
	echo "<br> Numero : ".$numero;
	echo "<br> Apellido: ".$apellido;
	echo "<br> Nombre: ".$nombre;
}else{
	echo "No hay datos con GET";
}


?>