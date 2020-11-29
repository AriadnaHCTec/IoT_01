<?php
	include "conexion.php";
	if ($_GET){
		$temperatura = $_GET["temperatura"];
		$claveUsuario = $_GET["claveUsuario"];
		$modelo = $_GET["modelo"];

		$sql_agregar = "INSERT INTO MedicionTemperaturaCorporal(temperatura, claveUsuario, modelo) VALUES (?,?,?,?,?,?,?,?)";
		$sentencia_agregar = $pdo->prepare($sql_agregar);
		$resultado = $sentencia_agregar->execute(array($temperatura, $claveUsuario, $modelo));
				
		if ($resultado){
			$sentencia_agregar = null;
			$pdo = null;
			echo "Datos guardados correctamente";
		}else{
			echo "<h1>Error al insertar en la B</h1>";
			echo $resultado;
		}
	}else{
		echo "<br>No hay datos con GET";
	}
?>