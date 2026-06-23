<?php

$csv = "resumen_mortalidad_por_pais.csv";

$datos = [];

if (($handle = fopen($csv, "r")) !== FALSE) {

    $header = fgetcsv($handle);

    while (($row = fgetcsv($handle)) !== FALSE) {
        $datos[] = [
            "pais" => $row[0],
            "mort" => floatval($row[1])
        ];
    }

    fclose($handle);
}

usort($datos, function($a, $b) {
    return $b["mort"] <=> $a["mort"];
});

$min = min(array_column($datos, "mort"));
$max = max(array_column($datos, "mort"));

if ($_SERVER["REQUEST_METHOD"] === "POST") {

    $valor = 0;

    if (!empty($_POST["pais"])) {

        $paisSeleccionado = $_POST["pais"];

        foreach ($datos as $fila) {

            if ($fila["pais"] === $paisSeleccionado) {

                $mort = $fila["mort"];

                if ($max == $min) {
                    $valor = 20;
                } else {

                    $valor = round(
                        1 + (($mort - $min) / ($max - $min)) * 19
                    );

                    $valor = max(1, min(20, $valor));
                }

                break;
            }
        }
    }

    file_put_contents("valor.txt", $valor);
}

?>
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<title>Mortalidad por país</title>
</head>
<body>

<h2>Selecciona un país</h2>

<form method="POST">

<select name="pais">

<option value="">-- Ninguno --</option>

<?php foreach($datos as $fila): ?>

<option value="<?= htmlspecialchars($fila["pais"]) ?>">
    <?= htmlspecialchars($fila["pais"]) ?>
    (<?= number_format($fila["mort"], 2) ?>)
</option>

<?php endforeach; ?>

</select>

<button type="submit">
Enviar
</button>

</form>

</body>
</html>
