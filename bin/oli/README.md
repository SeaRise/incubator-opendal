# oli

## What `oli` is

`oli` stands for OpenDAL Command Line Interface. It aims to provide a unified and user-friendly way to manipulate data stored in various storage services such as Object storage services(s3, azblob, gcs, minio, etc.), Key-Value services(Redis, etcd, TiKV, etc.), Filesystem-like service(HDFS, Azdfs, etc.), and also [all the other supported services](https://opendal.apache.org/docs/category/services).

## How to use `oli`

`oli` provide basic sub-commands like `oli ls`, `oli cat`, `oli stat`, `oli cp` and `oli rm`, just like what you use on your local filesystem.

### Install `oli`

`oli` could be installed by `cargo`:

```bash
cargo install oli --all-features
```

> `cargo` is the Rust package manager. `cargo` could be installed by following the [Installation](https://www.rust-lang.org/tools/install) from Rust official website.

### Configure `oli` Profile

`oli` requires a config file to work, it should be:

- `~/.config/oli/config.toml` on Linux
- `~/Library/Application Support/oli/config.toml` on macOS
- `C:\Users\<UserName>\AppData\Roaming\oli\config.toml` on Windows

The content of `config.toml` should be follow these pattern:

```toml
[profiles.<profile_name>]
configuration_key1 = "value1"
configuration_key2 = "value2"

[profiles.<another_profile_name>]
configuration_key3 = "value3"
configuration_key4 = "value4"

```

Here is an example of `config.toml`:

```toml
[profiles.s3]
type = "s3"
root = "/assets"
bucket = "<bucket>"
region = "<region>"
endpoint = "https://s3.amazonaws.com"
access_key_id = "<access_key_id>"
secret_access_key = "<secret_access_key>"

[profiles.r2]
type = "s3"
root = "/assets"
bucket = "<bucket>"
region = "auto"
endpoint = "https://<account_id>.r2.cloudflarestorage.com"
access_key_id = "<access_key_id>"
secret_access_key = "<secret_access_key>"
```

For different services, you could find the configuration keys in the corresponding [service document](https://opendal.apache.org/docs/services/).

### Example: use `oli` to upload file to AWS S3

```text
$ oli cp ./update-ecs-loadbalancer.json s3://update-ecs-loadbalancer.json
$ oli ls s3://                                                           
fleet.png
update-ecs-loadbalancer.json
```

### Example: use `oli` copy file from S3 to R2

```text
$ oli cp s3://fleet.png r2://fleet.png
$ oli ls r2://
fleet.png
```

## Contribute to `oli`

Contribution is not only about code, but also about documentation, examples, and so on! 🚀

If you have any questions or suggestions about `oli`, please feel free to open an issue on GitHub.

As `oli` is a part of Apache OpenDAL, you should follow the [CONTRIBUTION](https://github.com/apache/incubator-opendal/blob/main/CONTRIBUTING.md) documentation. There are still lots works to do with `oli`, you could track them on this [GitHub Issue](https://github.com/apache/incubator-opendal/issues/422).
