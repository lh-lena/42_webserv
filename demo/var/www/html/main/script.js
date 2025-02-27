
document.getElementById('contact-form').addEventListener('submit', async (e) => {
    e.preventDefault();

    // Clear previous error messages
    document.getElementById('name_error').textContent = '';
    document.getElementById('email_error').textContent = '';
    document.getElementById('message_error').textContent = '';

    const name = document.getElementById('name').value;
    const email = document.getElementById('email').value;
    const message = document.getElementById('message').value;

    // Client-side validation
    let isValid = true;

    if (name.trim() === '') {
        document.getElementById('nameError').textContent = 'Name is required.';
        isValid = false;
    }

    if (!email.match(/^[^\s@]+@[^\s@]+\.[^\s@]+$/)) {
        document.getElementById('email_error').textContent = 'Invalid email address.';
        isValid = false;
    }

    if (message.trim() === '') {
        document.getElementById('message_error').textContent = 'Message is required.';
        isValid = false;
    }

    if (!isValid)
        return;

    console.log(name, email, message);

    // Send data to the server
    const response = await fetch('contact_handler.php', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ name, email, message }),
    });
    
    const result = await response.json();
    
    if (result.success) {
        alert(result.message);
        document.getElementById('contact-form').reset();
    } else {
        alert('Error: ' + result.error);
    }
});