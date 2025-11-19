# Automatic Deployment of P2P Node to Yandex Cloud

This script automates the process of deploying a P2P node to Yandex Cloud through Container Registry and Compute Cloud.

## Requirements

Before running the script, make sure you have the following installed:

1. [Yandex Cloud CLI](https://cloud.yandex.ru/docs/cli/quickstart)
2. Docker
3. Access to your Yandex Cloud with the necessary permissions

## Preparation

1. Authenticate with Yandex Cloud CLI:
   ```bash
   yc init
   ```

2. Make sure you are in the correct folder

## Usage

1. Make the script executable:
   ```bash
   chmod +x deploy/yandex-cloud-deploy.sh
   ```

2. Run the script:
   ```bash
   ./deploy/yandex-cloud-deploy.sh
   ```

## What the Script Does

1. Checks for the presence of required tools (yc, docker)
2. Builds the Docker image of the application
3. Configures Docker to work with Yandex Container Registry
4. Creates Container Registry (if it doesn't already exist)
5. Uploads the image to Container Registry
6. Creates a virtual machine with Container Optimized Image
7. Configures the virtual machine to run the application container

## Script Parameters

The script uses the following default parameters:
- Registry name: `p2p-registry`
- VM name: `p2p-node`
- Availability zone: `ru-central1-a`

These parameters can be changed in the script itself if necessary.

## After Deployment

After the script completes successfully, your P2P node will be running in the cloud. You can check its status with:

```bash
yc compute instance get --name p2p-node
```

To connect to the node via SSH, use:

```bash
yc compute instance ssh p2p-node
```

## Monitoring

To monitor the node status, you can use:

```bash
yc compute instance get serial-port-output --name p2p-node
```

This will show the boot and operation logs of the node.