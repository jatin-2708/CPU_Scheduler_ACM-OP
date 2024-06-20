const { ipcRenderer } = require('electron');

const sundarbutton = document.getElementById('sundarbutton');
const resultElement = document.getElementById('result');
const sidebox = document.getElementById('sidebox');
const averageTATElement = document.getElementById('taroundavg');
const averageWTElement = document.getElementById('twaitavg');
const cpuOverheadElement = document.getElementById('cpuOverhead'); // Added for displaying CPU overhead

sundarbutton.addEventListener('click', () => {
    const algorithm = document.getElementById('definealgo').value;
    const input = document.getElementById('tempclass').value;
    console.log('Sending runScheduler event with algorithm:', algorithm);
    console.log('Input:', input);
    ipcRenderer.send('runScheduler', { algorithm, input });
});

ipcRenderer.on('schedulerResult', (event, { error, result }) => {
    if (error) {
        console.log(`Scheduler error: ${error}`);
        resultElement.innerText = `Error: ${error}`;
        resultElement.classList.add('text-danger');
        resultElement.classList.remove('text-success');
    } else {
        console.log('Scheduler result:', result);
        const { processes, taroundavg, twaitavg, schedulingOverhead } = result;
        resultElement.innerText = `Processes: ${JSON.stringify(processes, null, 2)}`;
        
        averageTATElement.innerText = `Overall Turnaround Time (Avg.): ${taroundavg}`;
        averageWTElement.innerText = `Overall Waiting Time (Avg.): ${twaitavg}`;
        cpuOverheadElement.innerText = `Scheduling Overhead: ${schedulingOverhead.toFixed(12)} s`; // Updated to show float value with units

        sidebox.style.display = 'block';
        resultElement.classList.add('text-success');
        resultElement.classList.remove('text-danger');
    }
});

