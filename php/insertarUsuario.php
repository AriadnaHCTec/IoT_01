<?php
	include "conexion.php";
	if ($_GET){
		$edad = $_GET["edad"];
		$nombre = $_GET["nombre"];
		$apellido = $_GET["apellido"];
		$fechaNacimiento = $_GET["fechaNacimiento"];
		$entidadFederativa = $_GET["entidadFederativa"];
		$municipio = $_GET["municipio"];
		$colonia = $_GET["colonia"];
		$calle = $_GET["calle"];

		$sql_agregar = "INSERT INTO usuario(edad, nombre, apellidos, fechaNacimiento, entidadFederativa, municipio, colonia, calle) VALUES (?,?,?,?,?,?,?,?)";
		$sentencia_agregar = $pdo->prepare($sql_agregar);
		$resultado = $sentencia_agregar->execute(array($edad, $nombre, $apellido, $fechaNacimiento, $entidadFederativa, $municipio, $colonia, $calle));
				
		if ($resultado){
			$sentencia_agregar = null;
			$pdo = null;
			echo "Datos guardados correctamente";
		}else{
			echo "<h1>Error al insertar en la BASE DE DATOS</h1>";
			echo $resultado;
		}
	}else{
		echo "<br>No hay datos con GET";
	}
?>