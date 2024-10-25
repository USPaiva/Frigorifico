document.getElementById('connectBtn').addEventListener('click', function() {
    startFetchingcontrol();
});


startFetchingData();

let humidityData = [];
let temperatureData = [];
let labels = [];  // Para armazenar os timestamps ou índices de dados
let previousData = { humidity: null, temperature: null}; //altitude: null, , co: null 

function startFetchingControl() {
    const command = { action: ledState ? 'off' : 'on' }; // Alterna entre 'on' e 'off'
    ledState = !ledState; // Atualiza o estado do LED
	document.getElementById('connectBtn').textContent = ledState ? 'Desligar Ventilador' : 'Ligar Ventilador';
    fetch('/led-control', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify(command)
    })
    .then(response => {
        if (!response.ok) {
            throw new Error('Network response was not ok');
        }
        return response.json();
    })
    .then(data => {
        console.log('Success:', data);
        // Aqui você pode adicionar lógica para atualizar a UI com o resultado, se necessário
    })
    .catch(error => {
        console.error('Error:', error);
    });
}


function startFetchingData() {
    setInterval(() => {
        fetch('/data')
        .then(response => response.json())
        .then(data => {
            // Atualizar valores numéricos na tela
            document.getElementById('humidityValue').textContent = data.Umid[data.Umid.length - 1].toFixed(2);
            document.getElementById('temperatureValue').textContent = data.Temp[data.Temp.length - 1].toFixed(2);

            // Atualizar gráficos
            const latestLabel = new Date().toLocaleTimeString();  // Usar timestamp como rótulo
            labels.push(latestLabel);
            if (labels.length > 10) labels.shift();  // Manter apenas os últimos 10 valores

            humidityData.push(data.Umid[data.Umid.length - 1]);
            if (humidityData.length > 10) humidityData.shift();

            temperatureData.push(data.Temp[data.Temp.length - 1]);
            if (temperatureData.length > 10) temperatureData.shift();

            updateCharts();
        });
    }, 1000);  // Atualizar a cada 1 segundo
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
            legend: {
                labels: {
                    color: 'white', // Muda a cor do texto da legenda
                }
            }
        },
        scales: {
            x: {
                ticks: {
                    color: 'white' // Cor das legendas do eixo X
                },
                display: true,
            },
            y: {
                ticks: {
                    color: 'white' // Cor das legendas do eixo X
                },
                beginAtZero: false
            }
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
            legend: {
                labels: {
                    color: 'white', // Muda a cor do texto da legenda
                }
            }
        },
        scales: {
            x: {
                ticks: {
                    color: 'white' // Cor das legendas do eixo X
                },
                display: true,
            },
            y: {
                ticks: {
                    color: 'white' // Cor das legendas do eixo X
                },
                beginAtZero: false
            }
        }
    }
});



// Função para atualizar os gráficos com novos dados
function updateCharts() {
    humidityChart.update();
    temperatureChart.update();
}
