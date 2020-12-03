<?php
	include "conexion.php";
	if ($_GET){
		$numero = $_GET["numero"];
		$apellido = $_GET["apellido"];
		$nombre = $_GET["nombre"];
		$extension = $_GET["extension"];
		$correo = $_GET["correo"];
		$oficina = $_GET["oficina"];
		$reporta = $_GET["reporta"];//Puede ser null
		$puesto = $_GET["puesto"];

		$sql_agregar = "INSERT INTO employees (employeeNumber, lastName, firstName, extension, email, officeCode, reportsTo, jobTitle) 
		VALUES (?,?,?,?,?,?,?,?)";
		$sentencia_agregar = $pdo->prepare($sql_agregar);
		$resultado = $sentencia_agregar->execute(array($numero, $apellido, $nombre, $extension, $correo, $oficina,$reporta, $puesto));
		


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