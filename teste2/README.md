#### Teste 2 - Disseminação de informação
###### Julianny Favinha Donda (156059)

O algoritmo é dividido em N processos. Cada processo é dividido em 2 threads, chamadas de _client_ e _server_. A thread _client_ se conecta (usando protocolo TCP) com algum outro processo, escolhido aleatoriamente, e envia a mensagem. A thread _server_ recebe a mensagem e responde se já possuia a mensagem ou não. Se o processo não possuia a informação, então é atualizado com a informação recebida. Note que um processo começa a disseminar somente após possuir a mensagem. Foi usado ZeroMQ para a troca de mensagens. As funções principais de zmq foram _recv_, para receber uma mensagem, e _send_, para enviar uma mensagem. No caso do _recv_, foi constatado que é uma função bloqueante, ou seja, o processo fica parado nessa função até receber uma mensagem de fato. Para contornar esse problema, foi necessário usar a flag NOBLOCK, que força o _recv_ a não bloquear, se não houver uma mensagem pronta para ser recebida. Em compensação, foi feito um loop com timeout para conseguir receber a mensagem. Além desse timeout, Tanto a thread _client_ quanto a thread _server_ possuem um timeout "global", para não ficarem escutando conexões nem enviando mensagens indefinidamente, pois espera-se que depois de um tempo, uma quantia razoável de processos já tenha a informação. Esse timeout foi escolhido de maneira mais ou menos proporcional ao número de processos. A linguagem escolhida foi Python.

Para N = 5 e k = 4, a saída do programa gossiping.py tem a seguinte estrutura:
![Output](images/gossiping-n5-k4.png)

#### Avaliação do algoritmo baseada em N e k
##### N = 500, k = 2 (_Vide arquivo gossiping-n500-k2.out_ ) 
**• Quantas vezes cada processo tentou fofocar (min, max, média)?** Min = 192 conexões, Max = 221 conexões, Média = 203.2 conexões
	
**• Qual parcela de tentativas de fofoca teve sucesso?** Foram realizadas 101599 conexões, e 498 sucessos. Logo, aproximadamente 0.5%.

**• No final da disseminação (quando todos os hosts pararam de fofocar), quantos processos tem a informação disseminada?** Ao final da disseminação, 499 processos possuem a informação.

**• Quanto tempo passou entre o início e o final da disseminação?** 512.8 segundos.

##### N = 500, k = 8 (_Vide arquivo gossiping-n500-k8.out_ )
**• Quantas vezes cada processo tentou fofocar (min, max, média)?** Min = 190 conexões, Max = 222 conexões, Média = 200.7 conexões
	
**• Qual parcela de tentativas de fofoca teve sucesso?** Foram realizadas 100338 conexões, e 497 sucessos. Logo, aproximadamente 0.5%.

**• No final da disseminação (quando todos os hosts pararam de fofocar), quantos processos tem a informação disseminada?** Ao final da disseminação, 498 processos possuem a informação.

**• Quanto tempo passou entre o início e o final da disseminação?** 512.6 segundos.

##### N = 1000, k = 2 (_Vide arquivo gossiping-n1000-k2.out_ )
**• Quantas vezes cada processo tentou fofocar (min, max, média)?** Min = 394 conexões, Max = 437 conexões, Média = 411.3 conexões
	
**• Qual parcela de tentativas de fofoca teve sucesso?** Foram realizadas 411318 conexões, e 996 sucessos. Logo, aproximadamente 0.2%.

**• No final da disseminação (quando todos os hosts pararam de fofocar), quantos processos tem a informação disseminada?** Ao final da disseminação, 997 processos possuem a informação.

**• Quanto tempo passou entre o início e o final da disseminação?** 1028.8 segundos.

##### N = 1000, k = 4 (_Vide arquivo gossiping-n1000-k4.out_ )
**• Quantas vezes cada processo tentou fofocar (min, max, média)?** Min = 393 conexões, Max = 438 conexões, Média = 410.2 conexões
	
**• Qual parcela de tentativas de fofoca teve sucesso?** Foram realizadas 410217 conexões, e 996 sucessos. Logo, aproximadamente 0.2%.

**• No final da disseminação (quando todos os hosts pararam de fofocar), quantos processos tem a informação disseminada?** Ao final da disseminação, 997 processos possuem a informação.

**• Quanto tempo passou entre o início e o final da disseminação?** 1027.1 segundos.