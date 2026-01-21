# REST API クイックスタート

このガイドでは、DigitShowDSTのREST APIを使用してセンサーデータにアクセスする方法を説明します。

## APIの有効化

1. `api_config.json` を編集して `enabled` を `true` に設定します：

```json
{
  "enabled": true,
  "host": "127.0.0.1",
  "port": 8080,
  "update_interval_ms": 100,
  "cors_enabled": true,
  "max_connections": 10
}
```

2. DigitShowDSTを起動します。APIサーバーは自動的に起動します。

## APIエンドポイント

### ヘルスチェック

サーバーが稼働中か確認します：

```bash
curl http://localhost:8080/api/health
```

レスポンス例：
```json
{
  "status": "ok",
  "timestamp": 1735768800000
}
```

### 現在のセンサーデータ取得

最新のセンサーデータのスナップショットを取得：

```bash
curl http://localhost:8080/api/sensor-data
```

レスポンス例：
```json
{
  "timestamp": 1735768800000,
  "physical_input": {
    "specimen": {
      "height_mm": 120.0,
      "area_mm2": 14400.0,
      "volume_mm3": 1728000.0,
      "weight_g": 3500.0,
      "box_weight_g": 10000.0
    },
    "shear_force_N": 125.3,
    "vertical_force_N": 450.8,
    "shear_displacement_mm": 2.45,
    "front_vertical_disp_mm": 0.82,
    "rear_vertical_disp_mm": 0.78,
    "front_friction_force_N": 5.2,
    "rear_friction_force_N": 5.1,
    "shear_stress_kpa": 10.5,
    "vertical_stress_kpa": 31.3,
    "normal_displacement_mm": 0.80,
    "tilt_mm": 0.02
  },
  "physical_output": {
    "front_ep_kpa": 100.0,
    "rear_ep_kpa": 100.0,
    "motor_rpm": 30.0
  }
}
```

### リアルタイムストリーム (SSE)

Server-Sent Eventsでリアルタイムデータを受信：

```bash
curl http://localhost:8080/api/sensor-data/stream
```

ストリーム出力例：
```
event: data
data: {"timestamp": 1735768800000, "physical_input": {...}, "physical_output": {...}}

event: data
data: {"timestamp": 1735768800100, "physical_input": {...}, "physical_output": {...}}
```

## プログラミング例

### JavaScript (ブラウザ)

```html
<!DOCTYPE html>
<html>
<head>
    <title>DigitShowDST リアルタイムモニタ</title>
    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
</head>
<body>
    <h1>せん断応力モニタ</h1>
    <canvas id="chart" width="800" height="400"></canvas>

    <script>
        const ctx = document.getElementById('chart').getContext('2d');
        const chart = new Chart(ctx, {
            type: 'line',
            data: {
                labels: [],
                datasets: [{
                    label: 'せん断応力 (kPa)',
                    data: [],
                    borderColor: 'rgb(75, 192, 192)',
                    tension: 0.1
                }]
            },
            options: {
                scales: {
                    x: { display: true, title: { display: true, text: '時刻' } },
                    y: { display: true, title: { display: true, text: 'τ (kPa)' } }
                }
            }
        });

        const eventSource = new EventSource('http://localhost:8080/api/sensor-data/stream');

        eventSource.addEventListener('data', (event) => {
            const data = JSON.parse(event.data);
            const time = new Date(data.timestamp).toLocaleTimeString();
            const stress = data.physical_input.shear_stress_kpa;

            chart.data.labels.push(time);
            chart.data.datasets[0].data.push(stress);

            // 直近100点のみ表示
            if (chart.data.labels.length > 100) {
                chart.data.labels.shift();
                chart.data.datasets[0].data.shift();
            }

            chart.update('none'); // アニメーションなし
        });

        eventSource.onerror = (error) => {
            console.error('SSE接続エラー:', error);
            eventSource.close();
        };
    </script>
</body>
</html>
```

### Python

```python
#!/usr/bin/env python3
"""
DigitShowDST センサーデータ取得サンプル
"""

import requests
import json
from datetime import datetime

def get_snapshot():
    """現在のスナップショットを取得"""
    response = requests.get('http://localhost:8080/api/sensor-data')
    data = response.json()

    print(f"タイムスタンプ: {datetime.fromtimestamp(data['timestamp']/1000)}")
    print(f"せん断応力: {data['physical_input']['shear_stress_kpa']:.2f} kPa")
    print(f"鉛直応力: {data['physical_input']['vertical_stress_kpa']:.2f} kPa")
    print(f"モーター回転数: {data['physical_output']['motor_rpm']:.1f} RPM")

def stream_data():
    """SSEストリームを受信"""
    response = requests.get(
        'http://localhost:8080/api/sensor-data/stream',
        stream=True
    )

    for line in response.iter_lines():
        if line.startswith(b'data: '):
            data = json.loads(line[6:])
            timestamp = datetime.fromtimestamp(data['timestamp']/1000)
            tau = data['physical_input']['shear_stress_kpa']
            sigma = data['physical_input']['vertical_stress_kpa']

            print(f"[{timestamp:%H:%M:%S}] τ={tau:6.2f} kPa, σ={sigma:6.2f} kPa")

if __name__ == '__main__':
    import sys

    if len(sys.argv) > 1 and sys.argv[1] == 'stream':
        print("ストリーム受信開始 (Ctrl+Cで終了)")
        try:
            stream_data()
        except KeyboardInterrupt:
            print("\n終了")
    else:
        get_snapshot()
```

実行例：
```bash
# スナップショット取得
python sensor_client.py

# ストリーム受信
python sensor_client.py stream
```

### Excel (VBA)

```vba
' Microsoft XML, v6.0 を参照設定に追加してください

Sub GetSensorData()
    Dim http As Object
    Set http = CreateObject("MSXML2.XMLHTTP")

    http.Open "GET", "http://localhost:8080/api/sensor-data", False
    http.send

    If http.Status = 200 Then
        Dim json As Object
        Set json = JsonConverter.ParseJson(http.responseText)

        Range("A1").Value = "タイムスタンプ"
        Range("B1").Value = json("timestamp")

        Range("A2").Value = "せん断応力 (kPa)"
        Range("B2").Value = json("physical_input")("shear_stress_kpa")

        Range("A3").Value = "鉛直応力 (kPa)"
        Range("B3").Value = json("physical_input")("vertical_stress_kpa")
    Else
        MsgBox "データ取得エラー: " & http.Status
    End If
End Sub
```

## トラブルシューティング

### ポート8080が使用できない

別のアプリケーションがポート8080を使用している場合は、`api_config.json` の `port` を変更してください：

```json
{
  "port": 8081
}
```

### 外部からアクセスできない

1. ファイアウォールの設定を確認
2. `api_config.json` の `host` を `"0.0.0.0"` に変更（セキュリティリスクに注意）

```json
{
  "host": "0.0.0.0"
}
```

### データが更新されない

- DigitShowDSTでボードが正常に初期化されているか確認
- Timer 1 (UI更新タイマー) が動作しているか確認
- ログファイルでエラーを確認

### CORSエラーが発生する

ブラウザからアクセスする場合、CORSが有効になっているか確認：

```json
{
  "cors_enabled": true
}
```

## 高度な使用例

### データをCSVに保存 (Python)

```python
import csv
import requests
from datetime import datetime

with open('sensor_log.csv', 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['Timestamp', 'Shear Stress (kPa)', 'Vertical Stress (kPa)', 'Motor RPM'])

    response = requests.get('http://localhost:8080/api/sensor-data/stream', stream=True)
    for line in response.iter_lines():
        if line.startswith(b'data: '):
            data = json.loads(line[6:])
            writer.writerow([
                datetime.fromtimestamp(data['timestamp']/1000).isoformat(),
                data['physical_input']['shear_stress_kpa'],
                data['physical_input']['vertical_stress_kpa'],
                data['physical_output']['motor_rpm']
            ])
            f.flush()  # リアルタイム書き込み
```

### WebSocketプロキシ (Deno)

SSEをWebSocketに変換するプロキシサーバー：

```typescript
// ws_proxy.ts
import { serve } from "https://deno.land/std/http/server.ts";

const SSE_URL = "http://localhost:8080/api/sensor-data/stream";

serve((req) => {
  if (req.headers.get("upgrade") !== "websocket") {
    return new Response("Upgrade to WebSocket required", { status: 400 });
  }

  const { socket, response } = Deno.upgradeWebSocket(req);

  socket.onopen = async () => {
    const sseResponse = await fetch(SSE_URL);
    const reader = sseResponse.body?.getReader();
    const decoder = new TextDecoder();

    while (true) {
      const { done, value } = await reader!.read();
      if (done) break;

      const text = decoder.decode(value);
      const lines = text.split('\n');

      for (const line of lines) {
        if (line.startsWith('data: ')) {
          socket.send(line.substring(6));
        }
      }
    }
  };

  return response;
}, { port: 3000 });
```

実行：
```bash
deno run --allow-net ws_proxy.ts
```

## 次のステップ

- [OpenAPI仕様](../docs/api/openapi.yaml) でAPIの詳細を確認
- [実装ガイド](./rest_api_guide.md) で内部実装を理解
- カスタムクライアントの開発

## 関連ドキュメント

- [knowledge/rest_api_guide.md](./rest_api_guide.md) - 実装詳細
- [docs/api/openapi.yaml](../docs/api/openapi.yaml) - OpenAPI仕様
- [README.md](../README.md) - プロジェクト概要
