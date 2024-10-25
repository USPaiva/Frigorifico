from Frigorifico import app
import subprocess
import platform
import psutil  # Biblioteca para checar processos de forma multiplataforma

def is_receptor_running():
    """Função para verificar se o receptor já está rodando."""
    for proc in psutil.process_iter(['pid', 'name', 'cmdline']):
        try:
            # Verifica se o cmdline é None ou se não contém 'receptor.py'
            cmdline = proc.info['cmdline']
            if cmdline is not None and 'receptor.py' in cmdline:
                return True
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
    return False

if __name__ == "__main__":
    # Verifica se o receptor já está rodando
    if not is_receptor_running():
        print("Iniciando o receptor MQTT...")
        if platform.system() == 'Windows':
            # No Windows, usamos "start" para abrir uma nova janela
            subprocess.Popen(['start', 'python', 'Frigorifico/receptor.py'], shell=True)
        else:
            # Em sistemas Unix (Linux/Mac), usamos "gnome-terminal" ou "xterm" para abrir um novo terminal
            subprocess.Popen(['gnome-terminal', '--', 'python3', 'Frigorifico/receptor.py'])
    else:
        print("Receptor MQTT já está rodando!")

    # Inicia o servidor Flask na thread principal
    app.run(host='0.0.0.0', port=5000, debug=True)
