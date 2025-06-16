from graphviz import Digraph

dot = Digraph(comment='Fluxograma CapSat', format='png')

# Início
dot.node('A', 'Início', shape='oval')

# Setup
dot.node('B', 'Inicializa comunicação serial', shape='parallelogram')
dot.node('C', 'Liga linha de alimentação 3V3', shape='rectangle')
dot.node('D', 'Exibe mensagem de inicialização', shape='rectangle')
dot.node('E', 'Inicializa componentes e WiFi', shape='rectangle')

# Loop principal
dot.node('F', 'Para cada ciclo de leitura:', shape='rectangle')

# Leituras dos sensores
dot.node('G1', 'Ler dados do relógio de tempo real (RTC)', shape='rectangle')
dot.node('G2', 'Ler dados do GPS', shape='rectangle')
dot.node('G3', 'Ler dados da câmera', shape='rectangle')
dot.node('G4', 'Ler dados do medidor de partículas', shape='rectangle')
dot.node('G5', 'Ler dados do umidímetro', shape='rectangle')
dot.node('G6', 'Ler dados do multímetro', shape='rectangle')
dot.node('G7', 'Ler dados do acelerômetro', shape='rectangle')
dot.node('G8', 'Ler dados do altímetro', shape='rectangle')
dot.node('G9', 'Ler dados do magnetômetro', shape='rectangle')
dot.node('G10', 'Ler dados do medidor de gás', shape='rectangle')
dot.node('G11', 'Ler dados do ozônoscópio', shape='rectangle')
dot.node('G12', 'Ler dados do pluviômetro', shape='rectangle')
dot.node('G13', 'Ler dados do termômetro', shape='rectangle')
dot.node('G14', 'Ler dados do radiômetro UV', shape='rectangle')

# Decisão de salvamento
dot.node('H', 'Tempo para salvar dados atingido?', shape='diamond')
dot.node('I', 'Atualiza tempo do último salvamento', shape='rectangle')
dot.node('J', 'Salva dados em arquivo (CSV ou JSON)', shape='rectangle')
dot.node('K', 'Exibe dados coletados', shape='rectangle')
dot.node('L', 'WiFi está conectado?', shape='diamond')
dot.node('M', 'Tempo para envio HTTP atingido?', shape='diamond')
dot.node('N', 'Envia dados para o servidor via HTTP', shape='rectangle')
dot.node('O', 'Atualiza tempo do último envio HTTP', shape='rectangle')

# Ligações do setup
dot.edge('A', 'B')
dot.edge('B', 'C')
dot.edge('C', 'D')
dot.edge('D', 'E')
dot.edge('E', 'F')

# Leituras dos sensores em sequência
dot.edge('F', 'G1')
dot.edge('G1', 'G2')
dot.edge('G2', 'G3')
dot.edge('G3', 'G4')
dot.edge('G4', 'G5')
dot.edge('G5', 'G6')
dot.edge('G6', 'G7')
dot.edge('G7', 'G8')
dot.edge('G8', 'G9')
dot.edge('G9', 'G10')
dot.edge('G10', 'G11')
dot.edge('G11', 'G12')
dot.edge('G12', 'G13')
dot.edge('G13', 'G14')
dot.edge('G14', 'H')

# Decisões e ações
dot.edge('H', 'I', label='Sim')
dot.edge('H', 'F', label='Não')
dot.edge('I', 'J')
dot.edge('J', 'K')
dot.edge('K', 'L')
dot.edge('L', 'M', label='Sim')
dot.edge('L', 'F', label='Não')
dot.edge('M', 'N', label='Sim')
dot.edge('M', 'F', label='Não')
dot.edge('N', 'O')
dot.edge('O', 'F')

# Renderiza o fluxograma
dot.format = 'png'
dot.render('fluxograma_capsat', view=False)
