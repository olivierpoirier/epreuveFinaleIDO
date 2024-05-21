all:relPtBC 

relPtBC:partieBoutonEtClient.c
	gcc -g -o partieBoutonEtClient partieBoutonEtClient.c -lpigpio

runPtBC:partieBoutonEtClient
	./partieBoutonEtClient

relPtMS:partieMatriceEtServeur.c
	gcc -g -o partieMatriceEtServeur partieMatriceEtServeur.c -lpigpio

runPtMS:partieMatriceEtServeur
	./partieMatriceEtServeur

relFinal:partieFinale.c
	gcc -g -o partieFinale partieFinale.c -lpigpio -lpthread -lm

runFinal:partieFinale
	./partieFinale

relMat:led8.c
	gcc -g -o led8 led8.c -lpigpio

runMat:led8
	./led8