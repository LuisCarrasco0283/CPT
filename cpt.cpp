// Algoritmo para ejecutar la prueba de desempeño continuo T.O.V.A.
// Autor: Ing. Luis Eduardo Cruz Carrasco
// Fecha: 16 de noviembre del 2023

// ---------------------------------------------------------¬
// Método: T.O.V.A											|
// Estímulo: 12 letters										|
// Objetivo: A - X											|
// Porcenta de pesentación 10% - 15%						|
// Display Time: 200 ms										|
// ISI: 1.5 seg												|
// Intentos/duración: 400/15 min							|
// ---------------------------------------------------------|


// Inclusión de librerías
#include<opencv2/core/core.hpp>;
#include<opencv2/highgui/highgui.hpp>;
#include<opencv2/imgproc.hpp>;
#include<Eigen/Dense>;
#include<iostream>;
#include<chrono>;
#include<fstream>;
#include<sstream>
#include<vector>
#include<thread>;
#include<numeric>;
#include<cmath>;
#include<algorithm>;
#include <direct.h>

// Declaración del nombre del usuario
std::string UserName;

// Declaración de variables para los índices medibles
double OmiError;									// Variable flotante para el error de omisión
double ComiError;								// Variable flotante para el error de comisión
double MeanTime;									// Variable flotante para el tiempo promedio de respuesta
double StdDev;									// Variable flotante para la desviación estándar del tiempo
int Trials;										// Total de eventos por suceder
double TotalTime;								// Tiempo total de las respuestas correctas

// Desclaración de variables locales
Eigen::MatrixXd Resultados;						// Matriz dinámica para almacenar los resultados esperados
std::vector<std::vector<long long>> Tiempos;	// Matriz dinámica para almacenar los tiempos de respuesta
std::vector<char> Secuencia;					// Vector fila que almacena la secuencia de caracteres por mostrar

// Devlaración de variables para las ventanas
cv::Mat Fondo;										// Variable del tipo matriz usada de background
cv::Mat CPT;										// Variable del tipo matriz usada para imprimir las letras
cv::Mat Wait;

// Lectura de los archivos .TXT [int]
Eigen::MatrixXd Data_Load(const std::string filename)
{
	// Declaración de variables locales
	std::vector<double> valores;				// Vector de valores adquiridos del .txt
	int filas = 0;								// Puntero indicador de las filas
	int columnas = 0;							// Puntero indicador de las columnas

	// Declaración de variables para recorrer el .txt
	std::ifstream archivo(filename);			// Carga del archivo .txt en una variable de la clase ifstream
	std::string linea;							// Variable del tipo string que recorrerá el archivo .txt

	// Ciclo para extraer los elementos del .txt
	while (std::getline(archivo, linea))		// Mientras hayan líneas [rows] por extraer
	{
		std::stringstream ss(linea);			// Objeto de la clase stringstream que recibe una cadena de caracteres
		double valor;							// Variable double para almacenar cada valor de la cadena de caracteres [1 by 1]
		char coma;								// Variable utilizada para evadir las comas que separan los datos del .txt


		// Ciclo para convertir los datos string del .txt en valores double
		while (ss >> valor)						// Mientra hayan valores por asignar [Asigana los valores dela cadena 1 por 1]
		{
			valores.push_back(valor);			// Se almacena en un vector cada valor double extraido del .txt [los agrega al final]
			if (filas == 0) ++columnas;			// Si la fila es la inicial suma una columna máspor cada valor asignado
			ss >> coma;							// Leer la coma
		}
		++filas;								// Suma una fila más si termina de asignar la línea anterior
	}

	// Ciclo para convertir el vector de valores en un objeto matriz de la clase Eigen
	Eigen::MatrixXd matriz(filas, columnas);			// Declaración de la matriz con tramaños de filas y columnas contadas

	for (Eigen::Index i = 0; i < filas; ++i)			// Para cada fila de 1 hasta n
	{
		for (Eigen::Index j = 0; j < columnas; ++j)		// Para cada columna de 1 hasta m
		{

			matriz(i, j) = valores[i * columnas + j];	// Asigna el correspondiente valor al elemento (row,col) de la matriz
		}
	}

	// Regresa la matriz constriuda como salida de la función Data_Load
	return matriz;
}

// Lectura de los archivos .TXT [string]
std::vector<char> Arreglo(const std::string filename)
{
	// Declaración de variables locales
	std::vector<char> valores;				// Vector de valores adquiridos del .txt
	int filas = 0;								// Puntero indicador de las filas
	int columnas = 0;							// Puntero indicador de las columnas

	// Declaración de variables para recorrer el .txt
	std::ifstream archivo(filename);			// Carga del archivo .txt en una variable de la clase ifstream
	std::string linea;							// Variable del tipo string que recorrerá el archivo .txt

	// Ciclo para extraer los elementos del .txt
	while (std::getline(archivo, linea))		// Mientras hayan líneas [rows] por extraer
	{
		std::stringstream ss(linea);			// Objeto de la clase stringstream que recibe una cadena de caracteres
		char valor;							// Variable double para almacenar cada valor de la cadena de caracteres [1 by 1]
		char coma;								// Variable utilizada para evadir las comas que separan los datos del .txt


		// Ciclo para convertir los datos string del .txt en valores double
		while (ss >> valor)						// Mientra hayan valores por asignar [Asigana los valores dela cadena 1 por 1]
		{
			valores.push_back(valor);			// Se almacena en un vector cada valor double extraido del .txt [los agrega al final]
			if (filas == 0) ++columnas;			// Si la fila es la inicial suma una columna máspor cada valor asignado
			ss >> coma;							// Leer la coma
		}
		++filas;								// Suma una fila más si termina de asignar la línea anterior
	}

	// Regresa la matriz constriuda como salida de la función Data_Load
	return valores;
}

// Función 
void DataSave(const std::vector<double> &Indices, const std::string UserName)
{
	// Se crea una carpeta para el usuario
	if (_mkdir(("C:\\TESIS\\DOCUMENTACIÓN\\CPT_FILES\\OUTPUTS\\" + UserName).c_str()) == -1)
	{
		std::cerr << "Error al crear la carpeta  " << std::endl;
	}
	else
	{
		std::cout << "Directorio creado";
	}
	
	// Crear el archivo con la data
	std::ofstream file(("C:\\TESIS\\DOCUMENTACIÓN\\CPT_FILES\\OUTPUTS\\" + UserName + "\\" + UserName + ".txt").c_str());

	if (file.is_open())
	{
		// Agregar elementos al archivo txt
		file << "Total de intentos:\n";
		file << Indices[0];
		file << "\nIntentos correctos: \n";
		file << Indices[1];
		file << "\nErrores totales: \n";
		file << Indices[2];
		file << "\nErrorespor comisión: \n";
		file << Indices[3];
		file << "\nErrores por omisión: \n";
		file << Indices[4];
		file << "\nTiempo total de respuestas correctas: \n";
		file << Indices[5];
		file << "\nMedia del tiempo de respuesta: \n";
		file << Indices[6];
		file << "\nDesviación estándar del tiempo de respuesta: \n";
		file << Indices[7];
		file.close();
		std::cout << "Archivo creado";
	}
	else
	{
		std::cerr << "No se pudo abrir el archivo";
	}

}

// Salvar Tiempos
void TimeSave(const std::vector<std::vector<long long>>& Tiempos, const std::string& UserName)
{
	// Abre el archivo en modo de escritura
	std::ofstream file("C:\\TESIS\\DOCUMENTACIÓN\\CPT_FILES\\OUTPUTS\\Tiempos_" + UserName + ".txt");

	// Verifica si el archivo se abrió correctamente
	if (!file)
	{
		std::cerr << "No se pudo abrir el archivo";
		return;
	}

	// Escribe cada tiempo en el archivo
	for (const auto& subvector : Tiempos)
	{
		for (const auto& tiempo : subvector)
		{
			file << tiempo << "\n";
		}
	}

	// Cierra el archivo
	file.close();
}

// Ciclo principal
int main()
{
	// Datos 
	UserName = "Prueba01";				// Definición del nombre de usuario
	Trials = 4;							// Definición del número de letras en la secuencia [máx = 200]
	
	// Carga del vector de resultados esperados
    Resultados = Data_Load("C:/TESIS/DOCUMENTACIÓN/CPT_FILES/INPUT/Resultados.txt");
	
	// Asignar el tamaño de la matriz de resultados a la matriz de respuestas
	Eigen::MatrixXd Respuestas(Resultados.rows(),Resultados.cols());

	// Carga del vector de la secuencia CPT-AX
	Secuencia = Arreglo("C:/TESIS/DOCUMENTACIÓN/CPT_FILES/INPUT/Secuencia_AX.txt");

	// Asignación de valores a las variables de la ventana
	Fondo = cv::Mat::zeros(1536, 864, CV_8U);			// Asignar una matriz de ceros para tener un fondo de color negro
	const cv::String Ventana("Ventana");				// Variable string que contenga la palabra "Ventana"

	// Configuración de la venta del CPT
	cv::namedWindow(Ventana, cv::WND_PROP_FULLSCREEN);												// Nombrar a la ventana 
	cv::setWindowProperty(Ventana, cv::WND_PROP_FULLSCREEN, cv::WindowFlags::WINDOW_FULLSCREEN);	// Configurar la venta en panatalla completa

	// Ciclo de espera
	for (int i = 5; i > 0; i--)
	{
		// Clonar los valores del fondo en la matriz CPT 
		Wait = cv::Mat::zeros(1536, 864, CV_8U);
		// Convertir un int a string
		std::string Numero = std::to_string(i);
		// Escribir texto sobre la matriz "CPT" [ventana,string,posición,fuente,grosor,escala,tamaño,tipo de línea,no invertir]
		putText(Wait, Numero, cv::Point(300, 800), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(255, 255, 255), 30, 8, false);
		// Mostrar en la ventana el fondo de "CPT" con el texto asignado
		imshow(Ventana, Wait);
		cv::waitKey(1000);
	}
	// Tiempro entre secuencias
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	// Ciclo para mostrar las letras en la ventana
	for (int i = 0; i < Trials; i++)			// Para cada letra en la cadena de secuencia
	{
		// Inicializar el contador del tiempo por cada iteración
		auto t0 = std::chrono::high_resolution_clock::now();
		// Clonar los valores del fondo en la matriz CPT 
		CPT = Fondo.clone();
		// Convertir a "string" cada elemento "char" del vector de secuencia
		std::string letra(1, Secuencia[i]);
		// Escribir texto sobre la matriz "CPT" [ventana,string,posición,fuente,grosor,escala,tamaño,tipo de línea,no invertir]
		putText(CPT, letra, cv::Point(300, 800), cv::FONT_HERSHEY_SIMPLEX, 10, cv::Scalar(255, 255, 255), 30, 8, false);
		// Mostrar en la ventana el fondo de "CPT" con el texto asignado
		imshow(Ventana, CPT);
		
		// Ciclo para medir el tiempo y temporizar el display
		auto t1 = std::chrono::high_resolution_clock::now();							// Inicia el contador del reloj
		cv::waitKey(100);																// Espera 100 [ms] con la letra del CPT mostrada
		imshow(Ventana, Fondo);															// Mostrar el fondo negro después de los 100 [ms]
		char Key = cv::waitKey(1500);													// Recibir un char o esperar 1.5 [seg]
		auto t2 = std::chrono::high_resolution_clock::now();							// Tomar el tiempo después de recibir char
		auto ms_int = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);	// Calcular la diferencia (t1 - t2) como reacción
		int waitTime = 1600 - ms_int.count();											// Calcular el tiempo faltante para los 1600 [ms]

		if (waitTime > 0)														// Si hay tiempo faltante
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(waitTime));	// Duerme el hilo durante el tiempo faltante
		}
		// Termina el ciclo del tiempo y temporizadores

		// Ingresar la respuesta del usuario en una matriz
		if ((Key - '0') != -49)							// Si se presiona una tecla
		{
			Respuestas(0, i) = int(Key - '0');				// El elemento i de la matriz será el valor ASCII de "Key"
			Tiempos.push_back({ ms_int.count() });			// El elemnto i del vector será el tiempo de respuesta
		}
		else												// si no se presiona tecla
		{
			Respuestas(0, i) = int(0);						// El elemento i de la matriz será 0
			Tiempos.push_back({ 0LL });						// El elemento i del vector será 0
		}

		auto tend = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
	}

	// Salvar el vector de tiempos
	TimeSave(Tiempos, UserName);

	// Calcular el porcenta de los errores de omisión
	OmiError = (Respuestas.array() == 0).count();

	// Calcular los errores por comisión
	ComiError = 0;
	for (int i = 0; i < Trials; i++)
	{
		if (Resultados(0,i) != Respuestas(0,i) and Respuestas(0,i) != 0)
		{
			ComiError++;
		}
	}

	// Estadística del tiempo
	long long TotalTime = 0;
	long long SqrSum = 0;
	size_t numElements = 0;

	for (const auto& subvector : Tiempos)
	{
		TotalTime += std::accumulate(subvector.begin(), subvector.end(), 0LL);
		SqrSum += std::inner_product(subvector.begin(), subvector.end(), subvector.begin(), 0LL);
		numElements += subvector.size();
	}

	double MeanTime = static_cast<double>(TotalTime) / numElements; // Calcula la media
	double variance = static_cast<double>(SqrSum) / numElements - MeanTime * MeanTime; // Calcula la varianza

	if (variance < 0) // Asegura que la varianza no sea negativa debido a errores de precisión de punto flotante
	{
		variance = 0;
	}

	double StdDev = std::sqrt(variance); // Calcula la desviación estándar

	// Variables para análisis
	double TotalError = OmiError + ComiError;

	double CorrectResponses = Trials - TotalError;

	// La media del tiempo de respuesta correcto
	MeanTime = TotalTime / (Tiempos.size() - (OmiError + ComiError));

	// La desviación estándar de los tiempos de rspuesta
	StdDev = std::sqrt(SqrSum / (CorrectResponses) - MeanTime * MeanTime);

	// Almacenamiento de la data
	std::vector<double> Indices;
	Indices.push_back(Trials);
	Indices.push_back(CorrectResponses);
	Indices.push_back(TotalError);
	Indices.push_back(ComiError);
	Indices.push_back(OmiError);
	Indices.push_back(TotalTime);
	Indices.push_back(MeanTime);
	Indices.push_back(StdDev);

	// Salvar la data
	DataSave(Indices, UserName);

	// Retorno 
	return 0;
}

// Algoritmo desarrollado a partir de [Greenberg & Waldman (1993)].
// Title: Developmental Normative Data on The Test of Variables of Attention.

