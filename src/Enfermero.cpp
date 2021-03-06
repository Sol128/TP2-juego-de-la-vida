
#include "Enfermero.h"

void Enfermero::evaluarCelulas(Lista<Tablero*>* losTableros, Lista<Portal*>* losPortales){
	losTableros->iniciarCursor();
	while(losTableros->avanzarCursor()){
		Tablero* unTablero;
		unTablero = losTableros->obtenerCursor();
		evaluarUnTablero(unTablero);
	}
}

void Enfermero::evaluarUnTablero(Tablero* unTablero){

	for(unsigned int columna = 1; columna <= unTablero->contarColumnas(); columna++){
		for(unsigned int fila = 1; fila <= unTablero->contarFilas(); fila++){
			Celula* unaCelula;
			unaCelula = unTablero->obtenerParcela(columna, fila)->obtenerCelula();
			unsigned int vecinasVivas = unTablero->obtenerCantidadDeVecinasVivas(columna, fila);

			if(unaCelula->estaViva()){
				if((vecinasVivas!=2)&&(vecinasVivas!=3)){
					unaCelula->infectarCelula();
				}
			}else{
				if(vecinasVivas==3){
					unaCelula->reanimarCelula();
				}
			}
		}
	}
}

void Enfermero::asignarColorPromedio(Tablero* unTablero,unsigned int columna, unsigned int  fila, Celula* unaCelula){
	unsigned int cantidadDeVecinas = 0;
	unsigned int cantidadTotalDeRojo = 0;
	unsigned int cantidadTotalDeVerde = 0;
	unsigned int cantidadTotalDeAzul = 0;

	unsigned int filaAnalizada = (fila-1);
	unsigned int ultimaFilaAnalizada = (fila+1);
	for(unsigned int i = filaAnalizada; i <= ultimaFilaAnalizada; i++){
		unsigned int columnaAnalizada = (columna-1);
		unsigned int ultimaColumnaAnalizada = (columna+1);
		for(unsigned int j = columnaAnalizada; j <= ultimaColumnaAnalizada; j++){
			bool esUnaPosicionDistinta = ((fila!=filaAnalizada)||(columna!=columnaAnalizada));
			if((unTablero->existeLaParcela(j, i)&&(esUnaPosicionDistinta))){
				Color* colorDeCelula;
				colorDeCelula = unTablero->obtenerParcela(j, i)->obtenerCelula()->getColor();
				cantidadTotalDeRojo += colorDeCelula->obtenerRojo();
				cantidadTotalDeVerde += colorDeCelula->obtenerVerde();
				cantidadTotalDeAzul += colorDeCelula->obtenerAzul();
				cantidadDeVecinas++;
			}
		}
	}

	unsigned int cantidadPromedioDeRojo = (cantidadTotalDeRojo/cantidadDeVecinas);
	unsigned int cantidadPromedioDeVerde = (cantidadTotalDeVerde/cantidadDeVecinas);
	unsigned int cantidadPromedioDeAzul = (cantidadTotalDeAzul/cantidadDeVecinas);

	unaCelula->cambiarColorDeLaCelula(cantidadPromedioDeRojo, cantidadPromedioDeVerde, cantidadPromedioDeAzul);
}


void Enfermero::aplicarEfectoDelPortal(Portal* unPortal, Grafo* elGrafo){
	Celula* celulaOrigen;
	celulaOrigen = unPortal->getParcelaOrigen()->obtenerCelula();
	Celula* celulaDestino;
	celulaDestino = unPortal->getParcelaDestino()->obtenerCelula();

	Arista* aristaDeIda;
	aristaDeIda =  elGrafo->buscarArista(unPortal->getNombreTableroOrigen(), unPortal->getNombreTableroDestino(), unPortal);

	if(unPortal->esPortalActivo()){
		Arista* aristaDeVuelta;
		aristaDeVuelta = elGrafo->buscarArista(unPortal->getNombreTableroDestino(), unPortal->getNombreTableroOrigen(), unPortal);
		aplicarEfectoActivo(celulaOrigen, celulaDestino, aristaDeIda, aristaDeVuelta);
	}
	else{
		if(unPortal->esPortalNormal()){
			aplicarEfectoNormal(celulaOrigen, celulaDestino, aristaDeIda);
		}
		else{
			aplicarEfectoPasivo(celulaOrigen, celulaDestino, aristaDeIda);
		}
	}
}

void Enfermero::aplicarEfectoActivo(Celula* celulaOrigen, Celula* celulaDestino, Arista* aristaDeIda, Arista* aristaDeVuelta){
	bool hayTransicionEnOrigen = (celulaOrigen->estaPorRevivir())||(celulaOrigen->estaPorMorir());
	if(hayTransicionEnOrigen){
		copiarEstadoEntreCelulas(celulaOrigen, celulaDestino);
		if(celulaOrigen->estaPorRevivir()){
			aristaDeIda->aumentarUnidadApeso();
		}

	}
	else{
		bool hayTransicionEnDestino = (celulaDestino->estaPorRevivir())||(celulaDestino->estaPorMorir());
		if(hayTransicionEnDestino){
			copiarEstadoEntreCelulas(celulaDestino, celulaOrigen);
			if(celulaDestino->estaPorRevivir()){
				aristaDeIda->aumentarUnidadApeso();
			}
		}
	}
}

void Enfermero::aplicarEfectoNormal(Celula* celulaOrigen, Celula* celulaDestino, Arista* aristaDeIda){
	bool hayTransicionEnOrigen = (celulaOrigen->estaPorRevivir())||(celulaOrigen->estaPorMorir());
	if(hayTransicionEnOrigen){
		copiarEstadoEntreCelulas(celulaOrigen, celulaDestino);
		if(celulaOrigen->estaPorRevivir()){
			aristaDeIda->aumentarUnidadApeso();
		}
	}
}

void Enfermero::aplicarEfectoPasivo(Celula* celulaOrigen, Celula* celulaDestino, Arista* aristaDeIda){
	if(celulaOrigen->estaPorRevivir()){
		if(!celulaDestino->estaViva()){
			celulaDestino->revivirCelula();
		}
		aristaDeIda->aumentarUnidadApeso();
	}
}

void Enfermero::copiarEstadoEntreCelulas(Celula* celulaModelo, Celula* celulaAmodificar){
	if(celulaModelo->estaPorRevivir()){
		if(!celulaAmodificar->estaViva()){
			celulaAmodificar->revivirCelula();
			Color* unColor;
			unColor = celulaModelo->getColor();
			celulaAmodificar->cambiarColorDeLaCelula(unColor->obtenerRojo(), unColor->obtenerVerde(), unColor->obtenerAzul());
		}
	}
	else{
		if(!celulaAmodificar->estaMuerta()){
			celulaAmodificar->matarCelula();
			celulaAmodificar->cambiarColorDeLaCelula(255, 255, 255); //Setea color blanco que representa una celula muerta
		}
	}
}

void Enfermero::actualizarCelulas(Lista<Tablero*>* losTableros, Lista<Portal*>* losPortales, Informe* losInformes, Grafo* elGrafo){
	unsigned int muertesEnElTurno = 0;
	unsigned int nacimientosEnElTurno = 0;

	establecerColoresDeLasCelulas(losTableros, losPortales, elGrafo);

	losTableros->iniciarCursor();
	while(losTableros->avanzarCursor()){
		Tablero* unTablero;
		unTablero = losTableros->obtenerCursor();
		actualizarUnTablero(unTablero, muertesEnElTurno, nacimientosEnElTurno);
	}

	actualizarInformes(losInformes, muertesEnElTurno, nacimientosEnElTurno);
}

void Enfermero::establecerColoresDeLasCelulas(Lista<Tablero*>* losTableros, Lista<Portal*>* losPortales, Grafo* elGrafo){

	losTableros->iniciarCursor();
	while(losTableros->avanzarCursor()){
		Tablero* unTablero;
		unTablero = losTableros->obtenerCursor();
		setearColoresEnUnTablero(unTablero);
	}

	losPortales->iniciarCursor();
	while(losPortales->avanzarCursor()){
		Portal* unPortal;
		unPortal = losPortales->obtenerCursor();
		aplicarEfectoDelPortal(unPortal, elGrafo);
	}
}

void Enfermero::setearColoresEnUnTablero(Tablero* unTablero){
	for(unsigned int columna = 1; columna <= unTablero->contarColumnas(); columna++){
		for(unsigned int fila = 1; fila <= unTablero->contarFilas(); fila++){
			Parcela* unaParcela;
			unaParcela = unTablero->obtenerParcela(columna, fila);
			Celula* unaCelula;
			unaCelula = unaParcela->obtenerCelula();
			if(unaCelula->estaPorRevivir()){
				asignarColorPromedio(unTablero, columna, fila, unaCelula);
			}
		}
	}
}

void Enfermero::actualizarUnTablero(Tablero* unTablero, unsigned int &muertesEnElTurno, unsigned int &nacimientosEnElTurno){
	for(unsigned int columna = 1; columna <= unTablero->contarColumnas(); columna++){
		for(unsigned int fila = 1; fila <= unTablero->contarFilas(); fila++){
			Parcela* unaParcela;
			unaParcela = unTablero->obtenerParcela(columna, fila);
			Celula* unaCelula;
			unaCelula = unaParcela->obtenerCelula();
			if(unaCelula->estaPorRevivir()){
				unaCelula->revivirCelula();
				unaCelula->aumentarEnergia(100*unaParcela->getTasaDeNatalidad());
				nacimientosEnElTurno++;
			}
			if(unaCelula->estaPorMorir()){
				unaCelula->matarCelula();
				muertesEnElTurno++;
				unaCelula->reducirEnergia(100*unaParcela->getTasaDeMortalidad());
				unaCelula->cambiarColorDeLaCelula(255, 255, 255); //Setea color blanco que representa una celula muerta
			}
		}
	}
}

void Enfermero::actualizarInformes(Informe* losInformes, unsigned int &muertesEnElTurno, unsigned int &nacimientosEnElTurno){
	unsigned int celulasVivas = (losInformes->getCelulasVivas()-muertesEnElTurno)+nacimientosEnElTurno;
	losInformes->setCelulasVivas(celulasVivas);
	losInformes->setNacimientos(nacimientosEnElTurno);
	losInformes->aumentarNacimientosTotales(nacimientosEnElTurno);

	losInformes->setMuertes(muertesEnElTurno);
	losInformes->aumentarMuertesTotales(muertesEnElTurno);

	losInformes->verificarCambios();
}

