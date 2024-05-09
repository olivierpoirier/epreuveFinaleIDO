all:relPtBC 

relPtBC:partieBoutonEtClient.c
	gcc -g -o partieBoutonEtClient partieBoutonEtClient.c -lpigpio

runPtBC:partieBoutonEtClient
	./partieBoutonEtClient

relPtMS:partieMatriceEtServeur.c
	gcc -g -o partieMatriceEtServeur partieMatriceEtServeur.c -lpigpio

runPtMS:partieMatriceEtServeur
	./partieMatriceEtServeur