const express = require("express");
const app = express();

app.listen(8080, () => console.log("Listening on port 8080"));

app.use(express.json());
app.use(express.static(__dirname + "/public"));
app.use(express.urlencoded({ extended: false }));

app.get("/", (req, res) => {
    res.send("Hello World!");
});