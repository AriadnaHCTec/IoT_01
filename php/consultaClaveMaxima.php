<?php
    include "conexion.php";

    $sql_obtener_usuario = "SELECT claveOximetria FROM MedicionOximetria";
    $sentencia_query = $pdo -> prepare($sql_obtener_usuario);
    $resultado = $sentencia_query -> execute();

    if ($resultado){
        echo "\nSe ha hecho la consulta del usuario más nuevo\n";
        while ($row = $sentencia_query -> fetch()){
            echo $row['claveOximetria'];
        }
        $sentencia_query = null;
        $pdo = null;
    } else{
        echo "Error al hacer consulta";
        echo $resultado;
    }

?>