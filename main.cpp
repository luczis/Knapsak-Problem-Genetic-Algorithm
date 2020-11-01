#include <iostream>
#include <iterator>
#include <vector>
#include <string>
#include <stdlib.h>
#include <time.h>
#include "deathsentences.hpp"

// Valores
const uint16_t itens_size = 12;
const uint16_t members_size = 100;
const float valores[itens_size] = {3,2,8,4,6,4,12,2,6,10,15,9};
const float pesos[itens_size] = {5,4,4,2,4,6,10,4,2,8,12,15};
const float peso_max = 36;
const float mutation_rate = 0.05;
const uint16_t geracoes_maximas = 100;
const uint16_t max_morte_torneio = 20;
const uint16_t grupo_size_torneio = 3;
const uint16_t sobreviventes_torneio = 2;

struct Item{
	int valor;
	int peso;	
};

struct Individual{
	uint16_t gene;
	int weight = 0;
	int value = 0;
	float score = 0;
};

void sort_members(std::vector<Individual>* members){
	// Tecnica utilizada: bubble-sort
	uint16_t position = 0;
	float score_0 = 0;
	float score_1 = 0;

	Individual temp_indv;

	while(position+1 < (uint16_t)members->size()){
		score_0 = (*members)[position].score;
		score_1 = (*members)[position+1].score;
		if(score_1 > score_0){
			temp_indv = (*members)[position];
			(*members)[position] = (*members)[position+1];
			(*members)[position+1] = temp_indv;
			if(position>0)
				position--;
		}
		else{
			position++;
		}
	}
}

float fit_function(Individual* indv, std::vector<Item> itens){
	indv->score = 0;

	// Calcula peso e valor
	indv->weight = 0;
	indv->value = 0;
	for(int i=itens.size();i>0;i--){
		if(indv->gene>>i & 1){
			indv->weight+=itens[i].peso;
			indv->value+=itens[i].valor;
		}
	}

	// Pontuacao igual a valor, caso esteja com peso correto
	if(indv->weight<=peso_max)
		indv->score = indv->value - 0.2f*indv->weight;

	return indv->score;
}

void kill_zeroes(std::vector<Individual>* members){
	// Elimina aqueles com pontuacao 0
	for(int i=0;i<(int)members->size();i++){
		if(!((*members)[i].score>0)){
			members->erase(members->begin()+i);
			i--;
		}
	}
}

void kill_members(std::vector<Individual>* members){
	kill_zeroes(members);

	// Torneio
	uint16_t rodadas_torneio = (rand() % (max_morte_torneio + 1))/(grupo_size_torneio-sobreviventes_torneio);

	for(int i=0; i<rodadas_torneio; i++){
		std::vector<Individual> Contestants;
		uint16_t contestants_positions[grupo_size_torneio];
		for(int j=0;j<grupo_size_torneio;j++)
			contestants_positions[j]=members->size();
		float total_score = 0.0f;
		int position = 0;
		bool old_position = true;

		// Escolhe os participantes
		for(int j=0;j<grupo_size_torneio;j++){
			old_position = true;
			while(old_position){
				position = rand()%(members->size());
				old_position = false;
				for(int k=0;k<grupo_size_torneio;k++)
					if(position == contestants_positions[k]){
						old_position = true;
						break;
					}
			}
			contestants_positions[j] = position;
			total_score += (*members)[position].score;
			Contestants.push_back((*members)[position]);
		}

		int membros_torneio[grupo_size_torneio];
		for(int j=0;j<grupo_size_torneio;j++)
			membros_torneio[j] = j;
		
		// Organiza nos melhores
		uint16_t member_position = 0;
		float score_0 = 0;
		float score_1 = 0;
		int temp_member;
		while(member_position+1 < grupo_size_torneio){
			score_0 = Contestants[membros_torneio[member_position]].score;
			score_1 = Contestants[membros_torneio[member_position+1]].score;
			if(score_1 > score_0){
				temp_member = membros_torneio[member_position];
				membros_torneio[member_position] = membros_torneio[member_position+1];
				membros_torneio[member_position+1] = temp_member;
				if(member_position>0)
					member_position--;
			}
			else{
				member_position++;
			}
		}

		// Melhores sao sobreviventes
		int vencedores[sobreviventes_torneio];
		for(int j=0;j<sobreviventes_torneio;j++)
			vencedores[j] = membros_torneio[j];

	/*	for(int j=0;j<grupo_size_torneio;j++)
			std::cout<<membros_torneio[j]<<"s"<<Contestants[membros_torneio[j]].score<<'\t';
		std::cout<<std::endl;

		for(int j=0;j<sobreviventes_torneio;j++)
			std::cout<<vencedores[j]<<"s"<<Contestants[vencedores[j]].score<<'\t';
		std::cout<<std::endl;*/

		// Apaga outros
		for(int j=0;j<grupo_size_torneio;j++){
			bool is_winner = false;
			for(int k=0;k<sobreviventes_torneio;k++)
				if(j==vencedores[k])
					is_winner = true;
			if(!is_winner){
				(*members)[contestants_positions[j]].score = 0.0f;
				//std::cout<< "Individuo " << contestants_positions[vencedores[rand()%sobreviventes_torneio]] << " matou o individuo " << contestants_positions[j] << " " << deathsentences[rand()%(deathsentences.size())] << "!" << std::endl;
			}
		}

		kill_zeroes(members);
	}
}

void repopulate_members(std::vector<Individual>* members){
	// Metade dos novos membros sao criancas, o resto aleatorio
	uint16_t children_size = (members_size - members->size()) / 2;

	float total_score = 0.0f;
	for(int i=0;i<(int)members->size();i++){
		total_score += (*members)[i].score;
	}

	for(int i=0; i<children_size; i+=2){
		int parent_1 = 0;
		int parent_2 = 0;

/*
		// Define first parent
		float roleta_val;
		roleta_val = (float)rand()/(float)RAND_MAX * total_score;
		for(float temp_score=0.0f; parent_1<(int)members->size() && temp_score<roleta_val; parent_1++)
			temp_score += (*members)[parent_1].score;

		// Define second parent
		do{
			roleta_val = (float)rand()/(float)RAND_MAX * total_score;
			for(float temp_score=0.0f; parent_2<(int)members->size() && temp_score<roleta_val; parent_2++)
				temp_score += (*members)[parent_2].score;
		} while(parent_1 == parent_2);
*/
		parent_1 = i;
		parent_2 = i+1;

		// Reprocriate
		Individual child_1;
		Individual child_2;

		// Crossover
		uint16_t pos_1 = rand()%itens_size;
		uint16_t pos_2;
		do{
			pos_2 = rand()%itens_size;
		} while(pos_1 == pos_2);
		uint16_t cross_mask = ((1<<(itens_size+1))-1) ^ ((1<<pos_1)-1) ^ ((1<<pos_2)-1);

		// Define genes
		child_1.gene = ((*members)[parent_1].gene & cross_mask) | ((*members)[parent_2].gene & !cross_mask);
		child_2.gene = ((*members)[parent_2].gene & cross_mask) | ((*members)[parent_1].gene & !cross_mask);

		members->push_back(child_1);
		members->push_back(child_2);
	}

	// Cria membros aleatorios
	for(int i=members->size();i<members_size;i++){
		Individual temp_indv;
		temp_indv.gene = rand() & ((1<<itens_size)-1);
		members->push_back(temp_indv);
	}
}

void mutate_members(std::vector<Individual>* members){
	for(int i=0;i<(int)members->size();i++){
		float rand_val = (float)rand()/(float)RAND_MAX;
		if(rand_val < mutation_rate){
			uint16_t gene_pos = rand()%itens_size;
			(*members)[i].gene = (*members)[i].gene ^ (1<<gene_pos);
		}
	}
}

int main(int argc, const char* argv[]) {
	srand(time(NULL));

	// Cria vetor de itens
	std::vector<Item> itens;
	for(int i=0;i<itens_size;i++){
		Item temp_item;
		temp_item.peso=pesos[i];
		temp_item.valor=valores[i];
		itens.push_back(temp_item);
	}

	std::vector<Individual> members;
	// Cria membros aleatorios
	for(int i=0;i<members_size;i++){
		Individual temp_indv;
		temp_indv.gene = rand() & ((1<<(itens_size+1))-1);
		members.push_back(temp_indv);
	}

	
	for(uint16_t generation = 0; generation<geracoes_maximas; generation++ ){
		// Calcula score
		for(int i=0;i<(int)members.size();i++){
			fit_function(&members[i], itens);
		}

		// Ordena membros
		sort_members(&members);
		
		// Output info
		std::cout<< "Geracao " << generation <<std::endl;
		std::cout
			<< "Melhor individuo: " << '\t' 
			<< "P:" << members[0].weight << '\t'
			<< "V:" << members[0].value << '\t'
			<< "G:";
		for(int i=itens_size;i>0;i--){
			std::cout<<(members[0].gene>>i & 1);
		}
		std::cout << '\t' << "S:" << members[0].score << std::endl;

		// Elimina membros
		kill_members(&members);

		// Repopula membros
		repopulate_members(&members);

		// Mutacao
		mutate_members(&members);

	}

	// Calcula score
	for(int i=0;i<(int)members.size();i++){
		fit_function(&members[i], itens);
	}

	// Ordena membros
	sort_members(&members);

	// Mostra os membros
	for(int i=0;i<(int)members.size();i++){
		std::cout
			<< i << '\t' 
			<< "P:" << members[i].weight << '\t'
			<< "V:" << members[i].value << '\t'
			<< "G:";
		for(int j=itens_size;j>0;j--){
			std::cout<<(members[i].gene>>j & 1);
		}
		std::cout << '\t' << "S:" << members[i].score << std::endl;
	}

	return 0;
}
