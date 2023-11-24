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
#include <cmath>;
#include <algorithm>;

// Declaración de variables para los índices medibles
float OmiError;									// Variable flotante para el error de omisión
float ComiError;								// Variable flotante para el error de comisión
float MeanTime;									// Variable flotante para el tiempo promedio de respuesta
float StdDev;									// Variable flotante para la desviación estándar del tiempo
int Trials;										// Total de eventos por suceder
double TotalTime;								// Tiempo total de las respuestas correctas

// Desclaración de variables locales
Eigen::MatrixXd Resultados;						// Matriz dinámica para almacenar los resultados esperados
std::vector<std::vector<long long>> Tiempos;	// Matriz dinámica para almacenar los tiempos de respuesta
std::vector<char> Secuencia;					// Vector fila que almacena la secuencia de caracteres por mostrar

// Devlaración de variables para las ventanas
cv::Mat Fondo;										// Variable del tipo matriz usada de background
cv::Mat CPT;										// Variable del tipo matriz usada para imprimir las letras

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
	for (int i = 0; i < filas; ++i)						// Para cada fila de 1 hasta n
		for (int j = 0; j < columnas; ++j)				// Para cada columna de 1 hasta m
			matriz(i, j) = valores[i * columnas + j];	// Asigna el correspondiente valor al elemento (row,col) de la matriz

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

// Función signo
double sign(double x) 
{
	return 1.0 / (1.0 + std::exp(-x));
}

// Ciclo principal
int main()
{
	// Carga del vector de resultados esperados
    Resultados = Data_Load("C:/Users/Luis Carrasco/Documents/BCI/Resultados.txt");
	Trials = 4;

	// Asignar el tamaño de la matriz de resultados a la matriz de respuestas
	Eigen::MatrixXd Respuestas(Resultados.rows(),Resultados.cols());

	// Carga del vector de la secuencia CPT-AX
	Secuencia = Arreglo("C:/Users/Luis Carrasco/Documents/BCI/Secuencia_AX.txt");

	// Asignación de valores a las variables de la ventana
	Fondo = cv::Mat::zeros(1536, 864, CV_8U);			// Asignar una matriz de ceros para tener un fondo de color negro
	const cv::String Ventana("Ventana");				// Variable string que contenga la palabra "Ventana"

	// Configuración de la venta del CPT
	cv::namedWindow(Ventana, cv::WND_PROP_FULLSCREEN);											// Nombrar a la ventana 
	cv::setWindowProperty(Ventana, cv::WND_PROP_FULLSCREEN, cv::WindowFlags::WINDOW_FULLSCREEN);// Configurar la venta en panatalla completa

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



		std::cout << Respuestas(0, i) << std::endl;
	
		std::cout << Tiempos[i][0] << std::endl;
		std::cout << ms_int.count() << std::endl;
		auto tend = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
		std::cout << "time:" << tend.count() << std::endl;
	}

	// Calcular el porcenta de los errores de omisión
	OmiError = (Respuestas.array() == 0).count();
	std::cout << "Errores por Omisión: " << OmiError << std::endl;

	// Calcular los errores por comisión
	ComiError = 0;
	for (int i = 0; i < Trials; i++)
	{
		if (Resultados(0,i) != Respuestas(0,i) and Respuestas(0,i) != 0)
		{
			ComiError++;
		}
	}

	TotalTime = 0;
	double SqrSum = 0;
	for (const auto& subvector : Tiempos) 
	{
		TotalTime += std::accumulate(subvector.begin(), subvector.end(), 0LL);
		SqrSum += std::inner_product(subvector.begin(), subvector.end(), subvector.begin(), 0LL);
	}

	// La media del tiempo de respuesta correcto
	MeanTime = TotalTime / (Tiempos.size() - (OmiError + ComiError));

	// La desviación estándar de los tiempos de rspuesta
	StdDev = std::sqrt(SqrSum / (Tiempos.size() - (OmiError + ComiError)) - MeanTime * MeanTime);

	std::cout << "Tiempo correcto: " << StdDev << std::endl;
	// Not necessary yet
	int TotalError = OmiError + ComiError;
	int CorrectResponses = Trials - TotalError;
	std::cout << "Errores totales: " << TotalError << std::endl;
	std::cout << "Respuestas correctas: " << CorrectResponses<< std::endl;
	std::cout << "Errores por comisión: " << ComiError << std::endl;












	// Retorno 
	return 0;
}

// Algoritmo desarrollado a partir de [Greenberg & Waldman (1993)].
// Title: Developmental Normative Data on The Test of Variables of Attention.

