#include "game.h"
#include <QRandomGenerator>
#include <QDebug>
#include <QtWidgets/QMessageBox>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlProperty>


/* <Construtor> Game::Game(QObject *parent): QObject(parent)
 *
 * Define as estruturas de dados com nomes vazios
 * para que o Hall da Fama seja preenchido com variáveis vazias
 * em todos os lugares que ainda não contenham informação válida
 * de jogadas passadas. As instâncias com playerName nulo ""
 * serão ignoradas pelos algoritmos de ordenação
*/

Game::Game(QObject *parent): QObject(parent)
{  
    playerName = "";
    playerMoves = 0;
    playerTime = 0;
    for(int i=0;i<HALL_OF_FAME_MAX+1;i++)
    {
        fileData[i].name = "";
        fileData[i].moves = 0;
        fileData[i].time = 0;
    }
    //carregar Hall da Fama. Se não houver um arquivo de nome hall_of_fame.pzl
    //ele será criado.
    loadHallOfFame();
    //O primeiro texto a ser exibido nesse local são as informações de jogo,
    //indicando ao usuário que ele deve fornecer seu nome e pressionar o botão
    //de iniciar nova partida.
    formHallOfFameText(hallOfFameText);
    playGameBtnText = "PLAY GAME";

    emit changedUpdateBoard();
}

Game::~Game()
{
    qDebug() << "Program ended";
}

/* bool Game::hasSolution()
 *
 * Verifica se o jogo é solucionável.
 * Número de inversões precisa ser PAR para ser solucionável,
 * uma vez que este é um quebra cabeça de 16 peças (N=4, par)
*/

bool Game::hasSolution()
{
    int numberOfInversions = 0;
    int linearPositions[NUMBER_OF_ROWS*NUMBER_OF_COLUMNS];

    //linearizar o vetor de posições
    //é mais simples trabalhar de forma unidimensional nessas funções
    for(int i=0; i<NUMBER_OF_ROWS; i++){
        for(int j=0;j<NUMBER_OF_COLUMNS;j++){
            linearPositions[convertToArray(i,j)] = positions[i][j];
        }
    }

    //percorrer o vetor linearizado procurando inversões
    for(int i=0; i<NUMBER_OF_ROWS*NUMBER_OF_COLUMNS; i++){
        for(int j=i+1;j<NUMBER_OF_ROWS*NUMBER_OF_COLUMNS;j++){
            if(linearPositions[i] != SENTINEL && linearPositions[j] != SENTINEL)
            {
                if(linearPositions[i]>linearPositions[j])
                    numberOfInversions++;
            }
        }
    }

    qDebug()<<"Number of inversions: " <<numberOfInversions;

    if(numberOfInversions % 2 == 0) return true;
    else return false;
}

/* void Game::shuffleGame()
 *
 * Embaralha as peças do jogo, da primeira à penúltima posição.
 * Deixa a última peça como um espaço em branco. Não verifica se é solucionável
 * (existe uma função para isso <Game::hasSolution()>)
 * Enxerga as posições de forma unidimensional na hora do sorteio
*/


void Game::shuffleGame()
{
    bool sorted[NUMBER_OF_ROWS*NUMBER_OF_COLUMNS];
    //array temporario. Preencher todos com falso
    //esses elementos serão mudados para verdadeiro à
    //medida que os números correspondentes forem
    //sorteados.
    for(int i=0; i<NUMBER_OF_ROWS*NUMBER_OF_COLUMNS; i++)
    {
        sorted[i] = false;
    }

    QRandomGenerator *generator =  QRandomGenerator::system();
    //os próprios recursos de seed do sistema serão utilizados para essa geração.

    for(int i=0; i<NUMBER_OF_ROWS; i++)
    {
        for(int j=0; j<NUMBER_OF_COLUMNS; j++)
        {
            if(i==NUMBER_OF_ROWS-1 && j==NUMBER_OF_COLUMNS-1) break;
            bool succeeded = false; //preso aqui até que um número válido seja sorteado
            while (succeeded == false)
            {
                //sorteia número entre 1 e o máximo
                int sortedNumber = generator->bounded(1,NUMBER_OF_ROWS*NUMBER_OF_COLUMNS);
                qDebug() << "sorted number: " << sortedNumber;

                //verifica se o número já foi sorteado
                if(sorted[sortedNumber-1]==false)
                {
                    sorted[sortedNumber-1]=true;
                    succeeded = true;
                    positions[i][j] = sortedNumber;
                }
            }
        }
    }
    //adiciona sentinela na última posição
    positions[NUMBER_OF_ROWS-1][NUMBER_OF_COLUMNS-1]=SENTINEL;

    //imprime sequencia (apenas para fins de debug)
    for(int i=0; i<NUMBER_OF_ROWS; i++)
    {
        for(int j=0; j<NUMBER_OF_COLUMNS; j++)
        {
            qDebug() << " " << positions[i][j];
        }
    }
}

/* void Game::swapTwo(int indexRowA, int indexColumnA, int indexRowB, int indexColumnB)
 *
 * Recebe índices bidimensionais de duas peças e as troca no vetor positions.
 * Não realiza nenhum tipo de verificação se a troca é permitida na lógica do jogo,
 * existem funções específicas para esse fim.
*/

void Game::swapTwo(int indexRowA, int indexColumnA, int indexRowB, int indexColumnB)
{
    playerMoves++;
    int copy = positions[indexRowA][indexColumnA];
    positions[indexRowA][indexColumnA] = positions[indexRowB][indexColumnB];
    positions[indexRowB][indexColumnB] = copy;
}

/* bool Game::isComplete()
 *
 * Verifica o vetor de posições, peça por peça, procurando os números de 1 a 15.
 * Retorna falso se qualquer diferença nessa sequência for encontrada.
 * Caso contrário, o jogo se encontra solucionado.
*/


bool Game::isComplete()
{
    for(int i=0; i<NUMBER_OF_ROWS; i++)
    {
        for(int j=0; j<NUMBER_OF_COLUMNS; j++)
            if((i!=NUMBER_OF_ROWS-1)||(j!=NUMBER_OF_COLUMNS-1))
                if(positions[i][j] != NUMBER_OF_COLUMNS*i+j+1) return false;
    }
    return true;
}

/* int Game::getEmptySpace(int indexLinha, int indexColuna)
 *
 * Recebe como parâmetros um índice bidimensional de uma peça.
 * Checa vizinhanças imediatas pelo espaço (sentinela)
 * Retorna o valor correspondente ao índice (na versão unidimensional)
 * Caso contrário, retorna o valor da sentinela.
*/

int Game::getEmptySpace(int indexLinha, int indexColuna)
{
    //Só procurar espaço acima das peças se elas não estiverem na primeira linha.
    if(indexLinha>0)
    {
        if(positions[indexLinha-1][indexColuna] == SENTINEL)
            return(convertToArray(indexLinha-1, indexColuna));
    }
    //Só procurar espaço abaixo das peças se elas não estiverem na última linha.
    if(indexLinha<NUMBER_OF_ROWS-1)
    {
        if(positions[indexLinha+1][indexColuna] == SENTINEL)
            return(convertToArray(indexLinha+1, indexColuna));
    }
    //Só procurar espaço à esquerda das peças se elas não estiverem na primeira coluna.
    if(indexColuna>0)
    {
        if(positions[indexLinha][indexColuna-1] == SENTINEL)
            return(convertToArray(indexLinha, indexColuna-1));
    }
    //Só procurar espaço à direita das peças se elas não estiverem na última coluna.
    if(indexColuna<NUMBER_OF_COLUMNS-1)
    {
        if(positions[indexLinha][indexColuna+1] == SENTINEL)
            return(convertToArray(indexLinha, indexColuna+1));
    }
    //caso contrário, não há espaço adjacente. Retorna sentinela.
    return SENTINEL;
}

/* int Game::convertToArray(int indexRow, int indexColumn)
 *
 * Função auxiliar. Recebe como parâmetro um índice bidimensional de uma peça
 * e retorna a sua conversão em índice unidirecional.
*/

int Game::convertToArray(int indexRow, int indexColumn)
{
    return(NUMBER_OF_COLUMNS*indexRow+indexColumn);
}

/* void Game::updateTexts()
 *
 * Cria os textos dos botões que serão usados na interface de acordo com
 * os valores das peças. Isso é usado para atualizar o tabuleiro e, se
 * for o caso, apagar a mensagem de "CONGRATULATIONS!" do jogo anterior.
*/

void Game::updateTexts()
{
    for(int i=0; i<4; i++){
        for(int j=0;j<4;j++){
            texts[convertToArray(i,j)] = QString::number(positions[i][j]);
        }
    }
}

/* QString Game::getTileTexts(int tileNumber)
 *
 * A ser usado pelo arquivo QML. Obtêm o valor do conteúdo de uma peça
 * (numérico ou não) a ser exibido no tabuleiro.
*/

QString Game::getTileTexts(int tileNumber)
{
    return texts[tileNumber];
}

/* void Game::loadHallOfFame()
 *
 * Tenta carregar o arquivo hall_of_fame.pzl. Caso ele não exista, ele
 * será criado.
*/

void Game::loadHallOfFame()
{
    QString puzzleFileName = "hall_of_fame.pzl";
    QFile loadFile(puzzleFileName);

    if(!loadFile.open(QIODevice::ReadOnly))
    {
        //não existe o arquivo. Criar um novo.
        saveHallOfFame();
        /*
         * apenas usar na versão widget
        QMessageBox mb;
        mb.setText("Could not open file.");
        mb.exec();
        return;
        */
        return;
    }

    QDataStream in(&loadFile);
    in.setVersion(QDataStream::Qt_4_5);

    //carrega toda a informação para o jogo
    for(int i=0; i<HALL_OF_FAME_MAX+1; i++)
    {
        //guarda no arquivo as informações de cada jogador
        in >> fileData[i].name;
        in >> fileData[i].moves;
        in >> fileData[i].time;
    }
    loadFile.close();
}

/* void Game::saveHallOfFame()
 *
 * Salva Hall da Fama de posse das informações do jogo.
 * A última jogada é salva em uma posição do Hall da Fama invisível para
 * o usuário, mas visível ao programa. Assim, a ordenação poderá levar esse
 * valor para as posições visíveis caso ele seja um dos melhores e a pior
 * estará contida na posição invisível e não será mais visualizada.
*/

void Game::saveHallOfFame()
{
    QString puzzleFileName = "hall_of_fame.pzl";
    QFile saveFile(puzzleFileName);
    if(!saveFile.open(QIODevice::WriteOnly))
    {
        /*
         * usar apenas na versão widget do jogo.
        QMessageBox mb;
        mb.setText("Could not open file.");
        mb.exec();
        return;
        */
    }

    QDataStream out(&saveFile);
    out.setVersion(QDataStream::Qt_4_5);

    //update players
    //salvar na última posição no Hall da fama
    fileData[HALL_OF_FAME_MAX].name = playerName;
    fileData[HALL_OF_FAME_MAX].moves = playerMoves;
    fileData[HALL_OF_FAME_MAX].time = playerTime;

    //ordenar hall da fama
    sortHallOfFame();

    //guardar informação de quantos jogadores há no hall
    for(int i=0; i<HALL_OF_FAME_MAX+1; i++)
    {
        //guardar no arquivo as informações de cada jogador
        out << fileData[i].name;
        out << fileData[i].moves;
        out << fileData[i].time;
    }
    saveFile.close();
}

/* void Game::sortHallOfFame()
 *
 * Função de ordenação do Hall da Fama. Utiliza o método de seleção com
 * dois critérios de seleção: movimento e tempo. Em caso de empate em
 * um deles, o outro será observado para definir a melhor entre duas
 * instâncias.
*/

void Game::sortHallOfFame()
{
    //copia conteúdos
    for(int i=0; i<HALL_OF_FAME_MAX+1; i++)
    {
        sortedByMoves[i].name = fileData[i].name;
        sortedByMoves[i].moves = fileData[i].moves;
        sortedByMoves[i].time = fileData[i].time;
        sortedByTime[i].name = fileData[i].name;
        sortedByTime[i].moves = fileData[i].moves;
        sortedByTime[i].time = fileData[i].time;
    }
    //primeiro, ordena por movimentos
    for(int i=0; i<HALL_OF_FAME_MAX+1; i++)
    {
        int best = i;
        for (int j=i+1; j<HALL_OF_FAME_MAX+1; j++)
        {
            if(compareByMoves(sortedByMoves[j],sortedByMoves[best]))
                best = j;
        }
        //troca dois
        playerData copy = sortedByMoves [best];
        sortedByMoves [best] = sortedByMoves[i];
        sortedByMoves[i] = copy;
    }
    //depois, ordena por tempo
    for(int i=0; i<HALL_OF_FAME_MAX+1; i++)
    {
        int best = i;
        for (int j=i+1; j<HALL_OF_FAME_MAX+1; j++)
        {
            if(compareByTime(sortedByTime[j],sortedByTime[best]))
                best = j;
        }
        //troca dois
        playerData copy = sortedByTime [best];
        sortedByTime [best] = sortedByTime[i];
        sortedByTime[i] = copy;
    }
    //fileData recebe apenas os que chegaram ao Hall da Fama
    //se a [ultima entrada nao passou, ela sera descartada.
    for(int i=0; i<HALL_OF_FAME_MAX; i++)
    {
        fileData[i].name = sortedByMoves[i].name;
        fileData[i].moves = sortedByMoves[i].moves;
        fileData[i].time = sortedByMoves[i].time;
    }

    //apaga os dados da posição invisível no Hall da Fama.
        fileData[HALL_OF_FAME_MAX].name="";
        fileData[HALL_OF_FAME_MAX].moves=0;
        fileData[HALL_OF_FAME_MAX].time=0;

}

/* bool Game::compareByMoves (playerData a, playerData b)
 *
 * Critério de seleção da ordenação que define se á é melhor
 * que b. Os critérios para que a seja considerado melhor
 * que b são, nessa ordem:
 * 1. Menor número de movimentos
 * 2. Menor tempo
*/

bool Game::compareByMoves (playerData a, playerData b)
{
    if(b.name == "..." || b.name == "" ) return true;
    if(a.name == "..." || a.name == "" ) return false;
    if(a.moves != b.moves)
        return (a.moves < b.moves);
    else
        return (a.time < b.time);
}

/* bool Game::compareByTime (playerData a, playerData b)
 *
 * Critério de seleção da ordenação que define se á é melhor
 * que b. Os critérios para que a seja considerado melhor
 * que b são, nessa ordem:
 * 1. Menor tempo
 * 2. Menor número de movimentos
*/

bool Game::compareByTime (playerData a, playerData b)
{
    if(b.name == "..." || b.name == "" ) return true;
    if(a.name == "..." || a.name == "" ) return false;
    if(a.time != b.time)
        return (a.time < b.time);
    else
        return (a.moves < b.moves);
}

/* void Game::startGame()
 *
 * Comando de início de um novo jogo. Dados de usuário são
 * zerados e o jogo é sorteado até que ele seja solucionável,
 * mas não esteja ainda resolvido. O temporizador é disparado.
*/

void Game::startGame()
{
    playGameBtnText = "START NEW GAME";
    //inicializa jogo
    playerMoves = 0;
    playerTime = 0;
    shuffleGame();
    //Abaixo, apenas para teste. Criar jogo quase pronto

    for(int i=0;i<4;i++){
        for(int j=0;j<4;j++){
            positions[i][j] = 4*i+j+1;
        }
    }
    positions[3][3]=15;
    positions[3][2]=16;


    while(!hasSolution() || isComplete()) //jogo impossível ou já resolvido
        shuffleGame();
    updateTexts();
    Q_EMIT changedUpdateBoard();
    playerClock.startTimer();

    Q_EMIT changedHOFContents();

}

/* void Game::endGame()
 *
 * Comando de término de um jogo, acionado quando este é resolvido.
 * O tempo contido no temporizador é transferido para a variável
 * correspondente. A mensagem "CONGRATULATIONS!" é formada no
 * tabuleiro. O botão de começar jogo exibe a pontuação do jogador.
*/

void Game::endGame()
{
    playerTime = playerClock.getSeconds();
    if(playerName == "")
        playerName = "Player";
    texts[0] = "C";
    texts[1] = "O";
    texts[2] = "N";
    texts[3] = "G";
    texts[4] = "R";
    texts[5] = "A";
    texts[6] = "T";
    texts[7] = "U";
    texts[8] = "L";
    texts[9] = "A";
    texts[10] = "T";
    texts[11] = "I";
    texts[12] = "O";
    texts[13] = "N";
    texts[14] = "S";
    texts[15] = "!";

    //Fazer o botão de começar jogo mostrar a pontuação do jogador
    playGameBtnText = "Moves: " + QString::number(playerMoves) + " / Time: "
            + QString::number(playerTime) + "s\n"
            + "Click to play again.";

    emit changedUpdateBoard();
    saveHallOfFame();
    formHallOfFameText(hallOfFameText);
    Q_EMIT changedHOFContents();
}

/* FUNÇÕES AUXILIARES QML
 *
 * A serem chamadas via comandos na interface gráfica, farão chamadas a
 * funções principais do jogo.
*/

void Game::buttonClickedOnDisplay(int buttonIndex)
{
    int clickedRow = buttonIndex / NUMBER_OF_COLUMNS;
    int clickedColumn = buttonIndex % NUMBER_OF_COLUMNS;
    buttonClicked(clickedRow,clickedColumn);
}

void Game::buttonClicked(int indexRow, int indexColumn)
{
    int theEmptySpace = getEmptySpace(indexRow, indexColumn);
    if( theEmptySpace == SENTINEL) //não há espaço livre
        return;
    else
    {
        int theEmptyRow = theEmptySpace / NUMBER_OF_COLUMNS;
        int theEmptyColumn = theEmptySpace % NUMBER_OF_COLUMNS;
        swapTwo(indexRow, indexColumn, theEmptyRow, theEmptyColumn);
    }
    updateTexts();
    emit changedUpdateBoard();
    if(isComplete()){
        endGame();
    }
}

QString Game::getHOFChange(){
    return (hallOfFameText);
}

void Game::getPlayerName(QString newName)
{
    playerName = newName;
}

void Game::formHallOfFameText(QString text)
{
    sortHallOfFame();
    text = "\nSorted by moves:\n\n";
    text += "Name\tMoves\tTime(s)\n";
    for(int x=0;x<HALL_OF_FAME_MAX;x++)
    {
        text += sortedByMoves[x].name + "\t" +
                  QString::number(sortedByMoves[x].moves)+
                "\t" + QString::number(sortedByMoves[x].time)+ "\n";
    }

    text += "\n\nSorted by time:\n\n";
    text += "Name\tMoves\tTime(s)\n";
    for(int x=0;x<HALL_OF_FAME_MAX;x++)
    {
        text += sortedByTime[x].name + "\t" +
                  QString::number(sortedByTime[x].moves) + "\t"
                    + QString::number(sortedByTime[x].time) + "\n";
    }
    hallOfFameText = text;
    Q_EMIT changedHOFContents();
}

QString Game::getStartGameBtnText()
{
    return playGameBtnText;
}
