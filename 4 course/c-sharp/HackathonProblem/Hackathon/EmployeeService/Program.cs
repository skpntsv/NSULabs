using EmployeeService.Controllers;
using EmployeeService.Models;
using MassTransit;

var builder = WebApplication.CreateBuilder(args);

var configuration = builder.Configuration;
builder.Services.AddSingleton<IConfiguration>(configuration);
builder.Services.Configure<Options>(configuration.GetSection("Options"));


builder.Services.AddSingleton<EmployeesReader>();


builder.Services.AddMassTransit(x =>
{
    x.AddConsumer<EmployeeConsumer>();

    x.UsingRabbitMq((context, cfg) =>
    {
        var rabbitMqUser = builder.Configuration["RabbitMQ:Username"];
        var rabbitMqPass = builder.Configuration["RabbitMQ:Password"];
        var rabbitMqHost = builder.Configuration["RabbitMQ:Host"];
        cfg.Host(rabbitMqHost, "/", h =>
        {
            h.Username(rabbitMqUser);
            h.Password(rabbitMqPass);
        });

        cfg.ReceiveEndpoint($"Employee_queue_{Guid.NewGuid()}",
            e => { e.ConfigureConsumer<EmployeeConsumer>(context); });
    });
});


// Создание приложения
var app = builder.Build();

// Запуск приложения
app.Run();