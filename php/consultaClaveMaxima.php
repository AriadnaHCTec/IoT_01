<?php
    include "conexion.php";
    $sql_obtener_usuario = "SELECT MAX(claveUsuario) AS claveMaxima FROM Usuario";
    $sentencia_query = $pdo -> prepare($sql_obtener_usuario);
    $resultado = $sentencia_query -> execute();

    if ($resultado){
        while ($row = $sentencia_query -> fetch()){
            echo $row['claveMaxima'];
        }
        $sentencia_query = null;
        $pdo = null;
    } else{
        echo "Error al hacer consulta";
        echo $resultado;
    }

?>