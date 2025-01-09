import random

# Secuencia de los 2 primeros dígitos
secuencia = ["00", "01", "02", "03", "04", "05", "06", "07"]

# Función para generar un dígito hexadecimal aleatorio
def digito_aleatorio():
    return format(random.randint(0, 255), '02X')

# Función para generar una línea de 50 dígitos
def generar_linea(indice):
    # Obtener el par de dígitos correspondiente a la secuencia
    par_digitos = secuencia[indice % len(secuencia)]
    
    # Generar los 48 dígitos hexadecimales aleatorios restantes
    digitos_aleatorios = ''.join(digito_aleatorio() for _ in range(24))
    
    # Construir la línea completa
    linea = par_digitos + digitos_aleatorios
    
    return linea

# Generar las 1024 líneas solicitadas
for i in range(4096):
    print(generar_linea(i))
