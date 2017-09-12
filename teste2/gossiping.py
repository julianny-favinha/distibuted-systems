"""
gossiping.py

Created by Julianny Favinha Donda
RA 156059
MC714 2s2017
"""

"""
REFERENCIAS

- https://stackoverflow.com/questions/29950824/pyzmq-permission-denied
- https://stackoverflow.com/questions/423379/using-global-variables-in-a-function-other-than-the-one-that-created-them
- https://stackoverflow.com/questions/7538988/zeromq-how-to-prevent-infinite-wait
- https://stackoverflow.com/questions/26012132/zero-mq-socket-recv-call-is-blocking
"""


from multiprocessing import Process, Value
from threading import Thread
from random import randrange
import zmq
import time
import sys

MESSAGE_TO_SEND = 'Lorem ipsum'

"""
Processo tem 1/k chance de parar. Seja um numero aleatorio de 0 a 100. se for <= 100/k, entao o processo deve parar e a funcao retorna True. caso contrario, o processo nao para e a funcao retorna False
"""
def stop(k):
	if randrange(101) <= 100/float(k):
		return True
	return False


"""
REPLY. Um processo fica escutando conexoes infinitamente, ate expirar o tempo (timeout). Se ainda nao possui a mensagem recebida, entao responde com 'success' e grava a mensagem. Caso contrario, responde com 'fail'. 
"""
def server(N, k, PORT_NUMBER, id_process):
	global message
	
	context = zmq.Context()
	socket = context.socket(zmq.REP)
	socket.bind('tcp://127.0.0.1:%s' % str(PORT_NUMBER + id_process))

	while time.time() < timeout:
		try:		
			msg = socket.recv(zmq.NOBLOCK)

			if message == '':
				#print "SERV: Sucesso. "+str(PORT_NUMBER + id_process)+" recebeu msg -"+msg+"-"
				socket.send('success')
				infected.value += 1
				message = msg
			else:
				#print "SERV: Falha. "+str(PORT_NUMBER + id_process)+" ja tinha a msg -"+message+"-"
				socket.send('fail')
		except:
			#print "SERV: Falha. "+str(PORT_NUMBER + id_process)+" timeout"
			time.sleep(0.5)

		time.sleep(1)

	socket.close()
	context.term()


"""
REQUEST. Um processo remetente se conecta com um processo destinatario, e tenta enviar a mensagem. Isso ocorre apenas se o remetente possui a mensagem a ser enviada. O destinatario responde 'success', caso conseguiu receber a mensagem, ou 'fail', caso em que ja possui a mensagem. Se ocorreu o caso 'fail', entao o remetente "joga o dado" com probabilidade 1/k usando a funcao stop() para verficar se deve parar ou nao de disseminar a mensagem. Essa thread continua ate expirar o tempo (timeout) ou ate o 'stop'.
"""
def client(N, k, PORT_NUMBER, id_process):
	global connections
	global successes

	#print "CLI: "+str(PORT_NUMBER + id_process)+" tem a message -"+message+"-"

	while time.time() < timeout:
		if message != '':
			context = zmq.Context()
			socket = context.socket(zmq.REQ)
			socket.linger = 0
			
			# tenta se conectar com algum processo diferente de si mesmo
			random_id = randrange(N)
			while(random_id == id_process):
				random_id = randrange(N)

			#print "CLI: "+str(PORT_NUMBER + id_process)+" conecta com "+str(PORT_NUMBER + random_id)
			socket.connect('tcp://127.0.0.1:%s' % str(PORT_NUMBER + random_id))
			socket.send(MESSAGE_TO_SEND)
			connections += 1		
			
			# timeout para conseguir receber a mensagem. a flag NOBLOCK nao deixa a funcao recv do socket bloquear, mas e necessario o loop para receber a mensagem de fato
			timeout_recv = time.time() + 10
			message_received = False
			while time.time() < timeout_recv and not message_received:
				try:
					message_server = socket.recv(flags=zmq.NOBLOCK)
					message_received = True
					socket.close()
					context.term()

					if message_server == 'success':
						successes += 1
					else:
						if stop(k):
							#print "CLI: "+str(PORT_NUMBER + id_process)+" ***parou***"
							break
						#else:
							#print "CLI: "+str(PORT_NUMBER + id_process)+" continua"
				except:
					time.sleep(0.5)

		time.sleep(1)

	if message != '':
		spread_time.value = time.time()

"""
	Funcao do processo. Cria duas threads, uma client e uma server
	message - guarda a mensagem que o processo tem. inicia com '' ate receber a mensagem de fato
	timeout - tempo para o processo expirar, ou seja, parar de enviar info para outros processos ou receber conexoes
	connections - quantidade de conexoes que o processo realizou com outro processos
	successes - quantidade de conexoes em que o processo obteve sucesso ao enviar a info para outro processo 
"""
def thread(spread_time, infected, sum_connections, min_connections, max_connections, N, k, PORT_NUMBER, id_process):
	global message
	global timeout
	global connections
	global successes
	
	message = ''
	timeout = time.time() + N
	connections = 0
	successes = 0

	# escolhe um processo com id N/2 para comecar com a mensagem
	if id_process == N/2:
		message = MESSAGE_TO_SEND

	th_client = Thread(target=client, args = (N, k, PORT_NUMBER, id_process))
	th_client.start()

	th_server = Thread(target=server, args = (N, k, PORT_NUMBER, id_process))
	th_server.start()

	th_client.join()
	th_server.join()

	sum_connections.value += connections
	if connections < min_connections.value:
		min_connections.value = connections

	if connections > max_connections.value:
		max_connections.value = connections

	print "Processo "+str(PORT_NUMBER+id_process)+" finalizou. Conectou "+str(connections)+" vezes. Sucesso: "+str(successes)


"""
	Main.
	Variaveis compartilhadas entre processos:
	- infected indica a quantidade de processos que receberam a mensagem, incluindo o processo que comecou a disseminar
	- sum_connections soma todas as conexoes realizadas entre processos
	- min_connections indica a menor quantidade de conexoes que um processo fez
	- max_connections indica a maior quantidade de conexoes que um processo fez
	- spread_time indica o tempo final de disseminacao da mensagem
"""
if __name__ == '__main__':
	start_time = time.time()	

	N = 1000
	k = 4
	PORT_NUMBER = 9000
	processes = []

	infected = Value('i', 1)
	sum_connections = Value('i', 0)
	min_connections = Value('i', N)
	max_connections = Value('i', 0)
	spread_time = Value('d', 0.0)

	for i in range(N):
		p = Process(target=thread, args=(spread_time, infected, sum_connections, min_connections, max_connections, N, k, PORT_NUMBER, i))
		processes.append(p)

	#print "Inicia gossiping de "+str(N)+" processos..."
	for p in processes:	
		p.start()

	for p in processes:
		p.join()

	elapsed_time = spread_time.value - start_time
	print "Processo que fofocou menos realizou "+str(min_connections.value)+" conexoes"
	print "Processo que fofocou mais realizou "+str(max_connections.value)+" conexoes"
	print "Qtd de conexoes realizadas: "+str(sum_connections.value)+" conexoes"
	print "Media de fofocas: "+str(sum_connections.value/float(N))+" conexoes/processo"	
	print "Qtd de processos que receberam a mensagem, incluindo quem comecou a disseminar: "+str(infected.value)
	print "Tempo decorrido entre o inicio e fim da disseminacao: "+str("{0:0.1f}".format(elapsed_time))+" s"
