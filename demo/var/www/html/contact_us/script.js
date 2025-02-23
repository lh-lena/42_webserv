
const form = document.getElementById('contact-form');

form.addEventListener('submit', async (e) => {
    e.preventDefault();

    // Clear previous error messages
    const errorSpans = document.querySelectorAll('.error');
    errorSpans.forEach(span => span.textContent = '');

    // const name = document.getElementById('name').value;
    // const email = document.getElementById('email').value;
    // const message = document.getElementById('message').value;

    // const messageDiv = document.querySelector('.message');
    // messageDiv.textContent = "";

    // // Client-side validation
    // let isValid = true;

    // if (name.trim() === '') {
    //     document.getElementById('nameError').textContent = 'Name is required.';
    //     isValid = false;
    // }

    // if (!email.match(/^[^\s@]+@[^\s@]+\.[^\s@]+$/)) {
    //     document.getElementById('email_error').textContent = 'Invalid email address.';
    //     isValid = false;
    // }

    // if (message.trim() === '') {
    //     document.getElementById('message_error').textContent = 'Message is required.';
    //     isValid = false;
    // }

    // if (!isValid)
    //     return;

    // console.log(name, email, message);

    const formData = new FormData(form);
    console.log(window.location);
    const req_url = window.location.protocol + '//' + window.location.host + '/test.py';
    // Send data to the server
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
        console.log(result.length);
    } catch (error) {
        console.error(error.message);
    }
    document.getElementById('contact-form').reset();
});