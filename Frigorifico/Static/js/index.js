let humidityData = [];
let temperatureData = [];
let fanData = [];  // Dados de acionamento do ventilador
let picoData = [];  // Dados de acionamento de irrigação
let labels = [];  // Para armazenar os timestamps ou índices de dados
let VentiladorState = false;
let previousData = { humidity: null, temperature: null, fan: null, pico: null };
let picoState = false;

document.getElementById('connectBtn').addEventListener('click', function() {
    startFetchingControl();
});

// Função para definir o limite de temperatura
function setTemperatureLimit() {
    const maxTemp = document.getElementById("maxTempInput").value;
    fetch('/set-temperature-limit', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ temperature: maxTemp })
    })
    .then(response => response.json())
    .then(data => console.log('Temperature limit set:', data))
    .catch(error => console.error('Error setting temperature limit:', error));
}

// Função para definir os horários do funcionamento modo turbo
function setPicoSchedule() {
    const onTime = document.getElementById("picoOn").value;
    const offTime = document.getElementById("picoOff").value;
    fetch('/set-pico-schedule', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ on: onTime, off: offTime })
    })
    .then(response => response.json())
    .then(data => console.log('pico schedule set:', data))
    .catch(error => console.error('Error setting pico schedule:', error));
}

// Função para controle manual do funcionamento modo turbo
function togglePico(action) {
    if(action==='on'){
        picoState=true;
        VentiladorState =true;
        document.getElementById('connectBtn').textContent = VentiladorState ? 'Desligar Ventilador' : 'Ligar Ventilador';
    }else{
        picoState=false;
        document.getElementById('connectBtn').textContent = VentiladorState ? 'Desligar Ventilador' : 'Ligar Ventilador';
    }
    fetch('/pico', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ action })
    })
    .then(response => response.json())
    .then(data => console.log('pico control:', data))
    .catch(error => console.error('Error controlling pico:', error));
}

// Função de controle do Ventilador
function startFetchingControl() {
    const command = { action: VentiladorState ? 'off' : 'on' }; // Alterna entre 'on' e 'off'
    VentiladorState = !VentiladorState; // Atualiza o estado do Ventilador
    document.getElementById('connectBtn').textContent = VentiladorState ? 'Desligar Ventilador' : 'Ligar Ventilador';
    if(VentiladorState=false){
        picoState=false;
    }
    fetch('/ventilador', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(command)
    })
    .then(response => response.json())
    .then(data => console.log('Ventilator control:', data))
    .catch(error => console.error('Error:', error));
}

// Função para buscar dados em intervalos
function startFetchingData() {
    setInterval(() => {
        fetch('/data')
        .then(response => response.json())
        .then(data => {
            
            const latestLabel = new Date().toLocaleTimeString();
            labels.push(latestLabel);
            if (labels.length > 10) labels.shift();

            humidityData.push(data.Umid[data.Umid.length - 1]);
            if (humidityData.length > 10) humidityData.shift();

            temperatureData.push(data.Temp[data.Temp.length - 1]);
            if (temperatureData.length > 10) temperatureData.shift();

            // Atualizar dados de ventilador e irrigação (simulados)
            fanData.push(data.Vent1[data.Vent1.length - 1]);
            if (fanData.length > 10) fanData.shift();
            picoData.push(data.Vent2[data.Vent2.length - 1]);  // Atualização manual de irrigação
            if (picoData.length > 10) picoData.shift();
            
            if(data.Vent1[data.Vent1.length - 1]=1){
                VentiladorState = true
            }else{
                VentiladorState = false
            }
            
            updateCharts();
        });
    }, 5000);
}

// Inicializar gráficos usando Chart.js
const humidityChart = new Chart(document.getElementById('humidityChart'), {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Umidade (g/m³)',
            data: humidityData,
            borderColor: 'rgba(54, 162, 235, 1)',
            borderWidth: 2,
            fill: false,
        }]
    },
    options: {
        plugins: {
            legend: { labels: { color: 'white' }}
        },
        scales: {
            x: { ticks: { color: 'white' }, display: true },
            y: { ticks: { color: 'white' }, beginAtZero: false }
        }
    }
});

const temperatureChart = new Chart(document.getElementById('temperatureChart'), {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Temperatura (°C)',
            data: temperatureData,
            borderColor: 'rgba(255, 99, 132, 1)',
            borderWidth: 2,
            fill: false,
        }]
    },
    options: {
        plugins: {
            legend: { labels: { color: 'white' }}
        },
        scales: {
            x: { ticks: { color: 'white' }, display: true },
            y: { ticks: { color: 'white' }, beginAtZero: false }
        }
    }
});

// Gráfico para acionamento do ventilador
const fanChart = new Chart(document.getElementById('fanChart'), {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Acionamento do Ventilador',
            data: fanData,
            backgroundColor: 'rgba(75, 192, 192, 0.2)',
            borderColor: 'rgba(75, 192, 192, 1)',
            borderWidth: 1,
        }]
    },
    options: {
        plugins: {
            legend: { labels: { color: 'white' }}
        },
        scales: {
            x: { ticks: { color: 'white' }},
            y: { ticks: { color: 'white' }}
        }
    }
});

// Gráfico para acionamento do modo turbo
const picoChart = new Chart(document.getElementById('picoChart'), {
    type: 'line',
    data: {
        labels: labels,
        datasets: [{
            label: 'Acionamento do modo de pico',
            data: picoData,
            backgroundColor: 'rgba(153, 102, 255, 0.2)',
            borderColor: 'rgba(153, 102, 255, 1)',
            borderWidth: 1,
        }]
    },
    options: {
        plugins: {
            legend: { labels: { color: 'white' }}
        },
        scales: {
            x: { ticks: { color: 'white' }},
            y: { ticks: { color: 'white' }}
        }
    }
});

// Função para atualizar todos os gráficos
function updateCharts() {
    humidityChart.update();
    temperatureChart.update();
    fanChart.update();
    picoChart.update();
}


document.addEventListener('DOMContentLoaded', () => {
    // Iniciar a busca por dados
    startFetchingData();
});
