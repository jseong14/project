const sql = require('mysql2');
const express = require('express');
const app = express();
app.use(express.json());

//DB 연결 구문
const db = sql.createConnection({
  host: 'localhost',
  user: 'root',
  password: '12345',
  database: 'example',
});
//

app.listen(8080, function () {
  console.log('listening on 8080')
});

app.set("view engine", "ejs")

app.get('/home', function (rq, rs) {
  rs.sendFile(__dirname + '/home.html')

});

app.get('/', function (rq, rs) {
  rs.sendFile(__dirname + "/home.html")
});

app.get('/data', function (rq, rs) {
  db.query("select * from autofarm order by date desc limit 10", (err, results) => {
    rs.render("data", { data: results })
  });
});

app.get('/graph', function (rq, rs) {
  db.query("select * from autofarm order by date desc limit 10", (err, results) => {
    const chartData = {
      soil: [],
      gas: [],
      light: [],
      temp: [],
      humi: [],
      date: [],
    };

    results.forEach((row) => {
      chartData.soil.push(row.soil);
      chartData.gas.push(row.gas);
      chartData.light.push(row.light);
      chartData.temp.push(row.temp);
      chartData.humi.push(row.humi);
      chartData.date.push(row.date);
    });

    rs.render('graph', { chartData }); // 템플릿에 데이터 전달
  });
});