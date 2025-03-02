
const form = document.getElementById('contact-form');

form.addEventListener('submit', async (e) => {
    e.preventDefault();

    const name = document.getElementById('name').value;
    const email = document.getElementById('email').value;
    const message = document.getElementById('message').value;

    const messageDiv = document.querySelector('.message');
    messageDiv.textContent = "";

    console.log(name, email, message); //rm

    const formData = new FormData(form);
    console.log(window.location);
    const req_url = window.location.protocol + '//' + window.location.host + '/contact_handler.py';
    // send data to the server
    try {
        const response = await fetch(req_url, {
            method: 'POST',
            // body: formData,
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name, email, message }),
        });
        if (!response.ok) {
            throw new Error(`Response status: ${response.status}`);
        }
        const result = await response.text();
        messageDiv.textContent = response.message;
        console.log(result.length);
        console.log(result);
    } catch (error) {
        messageDiv.textContent = response.message;
        console.error(error.message);
    }
    setTimeout(() => {
        document.getElementById('contact-form').reset();
        messageDiv.textContent = "";
    }), 1000;
});
