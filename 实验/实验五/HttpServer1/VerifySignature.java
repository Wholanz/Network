import java.security.KeyFactory;
import java.security.PublicKey;
import java.security.spec.X509EncodedKeySpec;

/**
 * Created by LK on 2015/6/16.
 */
public class VerifySignature {


    public boolean run(PublicKey pubkey, byte[] signed, String info) {
        try {
//            String pubkeyvalue = "30819f300d06092a864886f70d01010105";//这是GenerateKeyPair输出的公钥编码
            X509EncodedKeySpec bobPubKeySpec = new X509EncodedKeySpec(pubkey.getEncoded());


            KeyFactory keyFactory = KeyFactory.getInstance("RSA");
            PublicKey pubKey = keyFactory.generatePublic(bobPubKeySpec);


//            String info = "orderId=10dkfadsfksdkssdkd&amount=80&orderTime=20060519";
//            String info = "orderId=10dkfadsfksdkssdkd&amount=80&orderTime=20060509";
//            byte[] signed = hexStrToBytes("7e264f9bf6f6b884c9c2e2ecc1f6b2c5e7301593678cb73e008ad3184e9599b2b53d02555301aed5410c098698fe7613218d31f62cd9158cea69b5c6ae85b7755ef34f92395ddebe2be521660cd22ac93b2c7cec1fd9b0d4310a690bbf54464f0a3474c4fdb6c3946b8bf9f7b7a3a72e31861d5bf41b52e42027e4f1ce3a2587");//这是SignatureData输出的数字签名
            java.security.Signature signetcheck=java.security.Signature.getInstance("MD5withRSA");
            signetcheck.initVerify(pubKey);
            signetcheck.update(info.getBytes());
            if (signetcheck.verify(signed)) {
                System.out.println("info=" + info);
                System.out.println("签名正常");
                return true;
            }
            else {
                System.out.println("非签名正常");
                return false;
            }
        }
        catch (java.lang.Exception e) {e.printStackTrace();}

        return false;
    }

    public static final byte[] hexStrToBytes(String s) {
        byte[] bytes;

        bytes = new byte[s.length() / 2];

        for (int i = 0; i < bytes.length; i++) {
            bytes[i] = (byte) Integer.parseInt(s.substring(2 * i, 2 * i + 2),
                    16);
        }

        return bytes;
    }

}