# Scheduler 

## Overview
The Scheduler is a lightweight and efficient library designed to provide an easy-to-use API for priority-based execution of events. It is optimized for use on low-power devices, ensuring minimal resource usage while maintaining reliability and flexibility.

## Features 

- **Priority-Based Execution**: Schedule events with different priority levels to ensure critical tasks are executed first.

- **Lightweight**: Minimal resource footprint, ideal for low-power and resource-constrained environments.

- **Easy-to-Use API**: Simple and intuitive API for quick integration.

- **Flexible Event Handling**: Support for various event types and scheduling requirements.

## Use Cases
#### The Scheduler is suitable for :
- Embedded systems.
- IoT devices.
- Low power applications 
## How to use 
scheduler configuration
```C:examples/main.c [13-15]
```
adding event 
```C:examples/main.c [23-25]
```
suspending event
```C:examples/main.c [36-36]
```
resuming event
```C:examples/main.c [40-40]
```
deleting event
```C:examples/main.c [44-44]
```
## License
This project is licensed under the MIT License. See the **LICENSE** file for details

--- 

Feel free to report issues or suggest improvements via the issue tracker.
Happy scheduling.