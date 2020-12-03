<?php
	include "conexion.php";

	$numero = 1740;
	$apellido = 'Huesca';
	$nombre = 'Ariadna';
	$extension = '0x9273';
	$correo = 'A01749161@itesm.mx';
	$oficina = 6;
	$reporta = 1002;
	$puesto = 'President';

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
?>