using EmployeeService.Consumers;
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

        // Получение параметров из Options
        var options = builder.Configuration.GetSection("Options").Get<Options>();
        if (options != null)
        {
            var queueName = $"Employee_queue_{options.EmployeeType}_{options.EmployeeId}";

            cfg.ReceiveEndpoint(queueName, e => { e.ConfigureConsumer<EmployeeConsumer>(context); });
        }
        else
        {
            throw new Exception("Настройки Options не найдены!");
        }
    });
});


// Создание приложения
var app = builder.Build();

// Запуск приложения
app.Run();